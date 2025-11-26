
/* Parallel matrix multiplication program based on Fox's algorithm */
/* Reads the matrices X and Y from files, multiplies them and      */
/* writes the result to a file.                                    */
/* Mats Aspnäs 31.1.2000 */

/* Compile with   'mpicc -O3 fox.c matrixutil.o -o fox -lm'  */

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#include "matrixutil.h"

int min(int a, int b) {
  if (a<b) return(a);
  else return(b);
}

int main(int argc, char** argv) {

  int verbose = 0;                 /* Verbose flag, produces output */
  int debug = 0;                   /* Debug flag, produces even more output */
  int c, dlimit;
  double start;

  const int datatag = 42;      /* Tag for message passing */
  int nproc, id;               /* Nr of processes and own identifier */
  int i, j, k, l;              /* Loop indexes */
  int N;                       /* Size of global matrices */
  int N_local;                 /* Size of local matrices */
  int startx, starty;          /* Used when distributing/collecting data */

  float *X, *Y, *Z;                    /* Matrices to be multiplied */
  float *X_local, *Y_local, *Z_local;  /* Local submatrices */
  float *tmp;                          /* Temporary matrix, used in broadcast */
  char *fn1, *fn2, *fn3;               /* Filenames */

  MPI_Comm grid_comm;               /* Topology with grid structure */
  MPI_Comm row_comm, col_comm;      /* Communicators for row and column */
  int q;                            /* Process grid is of size q*q */
  int my_row, my_col;               /* Row and column number in process grid */
  int source, dest;       /* Source and destination addresses for circular shift */
  int grid_rank;                    /* Process rank in grid */
  int stage;
  int dimensions[2], wraparound[2];
  int coordinates[2], remain[2];
  MPI_Status status;

  /* Initialize MPI, get nr of processes and own id */
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  /* Parse arguments to see if we have a -v or -d flag */
  while ((c=getopt(argc, argv, "vd:")) != -1) {
    switch (c) {
    case 'v':
      verbose = 1;             /* Set verbose flag */
      break;
    case 'd':
      debug = 1;              /* Set both debug and verbose flags */
      verbose = 1;
      dlimit = atoi(optarg); /* Get the argument to -d  */
      break;
    }
  }

  /* The process grid will be of size q*q */
  q = (int) sqrt((double) nproc);

  /* Check that we have a square number of processes */
  if (q*q != nproc) {
    if (id == 0) {
      printf("You have to use a square number of processes\n");
      printf("Quitting\n"); fflush(stdout);
    }
    MPI_Finalize();
    exit(1);
  }

  if (verbose && (id == 0)) {
    printf("Using a process grid of size %d*%d\n", q,q);
    fflush(stdout);
  }

  /* Process 0 allocates space for filenames, reads the size of matrices */
  /* and reads the filenames */
  if (id == 0) {
    /* Allocate space for filenames */
    fn1 = (char *) malloc(sizeof(char)*80);
    fn2 = (char *) malloc(sizeof(char)*80);
    fn3 = (char *) malloc(sizeof(char)*80);
    
    printf("Give size of matrices:\n "); fflush(stdout);
    scanf("%d",&N);

    printf("Give names of two files with matrices to multiply: \n"); fflush(stdout);
    scanf("%s%s", fn1,fn2);
    printf("Give name of output file: \n"); fflush(stdout);
    scanf("%s", fn3);
    printf("\n"); fflush(stdout);
  }

  /* Broadcast the matrix size N to all processes */
  MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (verbose && (id == 0)) {
    printf("Broadcasted matrix size %d to all processes\n",N);
    fflush(stdout);
  }
  /* Calculate size of the local matrices in each process */
  N_local = N/q;

  /* Check that q divides N evenly */
  if (N_local*q != N) {
    if (id == 0) {
      printf("The matrix size (%d) is not evenly divisible ", N);
      printf("by the process grid size (%d)\n", q);
      printf("Quitting\n"); fflush(stdout);
    }
    MPI_Finalize();
    exit(1);
  }

  /* Process zero allocates space for matrices and reads them from the files */
  if (id == 0) {
    if (verbose) {
      printf("Local matrix size is %d\n", N_local);
      fflush(stdout);
    }

    /* Allocate space for matrices */
    X = (float *) malloc(sizeof(float)*N*N);
    Y = (float *) malloc(sizeof(float)*N*N);
    Z = (float *) malloc(sizeof(float)*N*N);

    /* Read the input matrices from the files */
    if (!fread_matrix(X, N*N, fn1)) {
      printf("error in reading file %s\n", fn1); fflush(stdout);
      exit(1); /* Should also terminate other processes */
    }
    if (!fread_matrix(Y, N*N, fn2)) {
      printf("error in reading file %s\n", fn2); fflush(stdout);
      exit(1);
    }
  }

  /* Print part of the matrix if debug is on */
  if (debug && (id == 0)) {
    int limit;
    limit = min(dlimit, N);
    printf("The %d*%d first entries in the matrix X is\n", limit,limit);
    fflush(stdout);
    write_matrix(X, limit);
  }

  /* Allocate space for the local matrices */
  X_local = (float *) malloc(sizeof(float)*N_local*N_local);
  Y_local = (float *) malloc(sizeof(float)*N_local*N_local);
  Z_local = (float *) malloc(sizeof(float)*N_local*N_local);

  /* Nr of processes per dimension */
  dimensions[0] = dimensions[1] = q;
  /* Make it cyclic in second dimension (columns will wrap around) */
  wraparound[0] = 0; wraparound[1] = 1;

  /* Create the process grid in the topology grid_comm */
  MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, wraparound, 0, &grid_comm);

  /* Get own rank in the grid communicator */
  MPI_Comm_rank(grid_comm, &grid_rank);

  /* Get own coordinates in the process grid */
  MPI_Cart_coords(grid_comm, grid_rank, 2, coordinates);
  my_row = coordinates[0];  /* Row index */
  my_col = coordinates[1];  /* Column index */

  /* Create communicators for rows */
  remain[0] = 0; remain[1] = 1;
  MPI_Cart_sub(grid_comm, remain, &row_comm);

  /* Create communicators for columns */
  remain[0] = 1; remain[1] = 0;
  MPI_Cart_sub(grid_comm, remain, &col_comm);

  if (verbose && (id == 0)) {
    printf("Process 0 distributing data to all processes\n");
    fflush(stdout);
  }

  /* Distribute matrices X and Y on the process grid */
  if (grid_rank == 0) {
    /* Process zero sends submatrices to all other processes */
    for (i=q-1; i>=0; i--) {
      for (j=q-1; j>=0; j--) {  /* For all processes in the 2-D grid */
	startx = i*N_local;     /* Index to the global matrix where */
	starty = j*N_local;     /* the submatrix of this process starts */
	/* Copy submatrices to the local matrices */
	for (k=0; k<N_local; k++) {
	  for (l=0; l<N_local; l++) {
	    X_local[k*N_local+l] = X[(startx+k)*N+(starty+l)];
	    Y_local[k*N_local+l] = Y[(startx+k)*N+(starty+l)];
	  }
	}
	/* Find destination address for the submatrices */
	coordinates[0] = i; coordinates[1] = j;
	MPI_Cart_rank(grid_comm, coordinates, &dest);

	/* Don't send to your self ! */
	if (dest != 0) {
	  if (verbose) {
	    printf("    sending to process %d\n", dest);
	    fflush(stdout);
	  }
	  MPI_Send(X_local, N_local*N_local, MPI_FLOAT, dest, datatag, 
		   grid_comm);
	  MPI_Send(Y_local, N_local*N_local, MPI_FLOAT, dest, datatag, 
		   grid_comm);
	}
      }
    }
  }
  /* All other processes receive the submatrices */
  else {
    MPI_Recv(X_local, N_local*N_local, MPI_FLOAT, 0, datatag, grid_comm,
	     &status);
    MPI_Recv(Y_local, N_local*N_local, MPI_FLOAT, 0, datatag, grid_comm,
	     &status);
  }

  /* Synchronize all processes before we proceed */
  MPI_Barrier(grid_comm);

  /* Print the local matrix in process 0 if debug flag is on */
  if (debug && (id == 0)) {
    int limit;
    limit = min(dlimit, N_local);
    printf("\nThe %d*%d first entries in the local matrix X in process 0 is\n",
	   limit, limit);
    fflush(stdout);
    write_matrix(X_local, limit);
  }

  /* Do the matrix multiplication algorithm */

  if (verbose && (id == 0)) {
    printf("Starting matrix multiplication\n");
    fflush(stdout);
  }

  /* Calculate source and destination addresses for the circular shift */
  dest = (my_row+q-1)%q;
  source = (my_row+1)%q;

  /* Allocate storage for temporary local matrix */
  tmp = (float *) malloc(sizeof(float)*N_local*N_local);

  start = MPI_Wtime();
  /* Set the matrix Z_local to zero */
  settozero(Z_local, N_local);

  for (stage=0; stage<q; stage++) {
    int bcast_root;
    if (verbose && (id == 0)) {
      printf("    stage %d\n", stage);
      fflush(stdout);
    }

    /* The process bcast_root does the broadcast in each stage */
    bcast_root = (my_row+stage)%q;
    if (bcast_root == my_col) {
      MPI_Bcast(X_local, N_local*N_local, MPI_FLOAT, bcast_root, row_comm);
      matrixmult(X_local, Y_local, Z_local, N_local);
    } else {
      MPI_Bcast(tmp, N_local*N_local, MPI_FLOAT, bcast_root, row_comm);
      matrixmult(tmp, Y_local, Z_local, N_local);
    }
    MPI_Sendrecv_replace(Y_local, N_local*N_local, MPI_FLOAT, dest, datatag, 
			 source, datatag, col_comm, &status);
  }

  if (id == 0) {
    printf("Time for matrix multiplication %6.1f seconds\n\n", 
		      MPI_Wtime()-start);
    fflush(stdout);
  }

  if (verbose && (id == 0)) {
    printf("Matrix multiplication done, collecting results\n");
    fflush(stdout);
  }

  /* Copy the local result in process 0 to the global result in Z */
  if (grid_rank == 0) {
    /* Copy the result from process 0 into the global matrix Z */
    for (k=0; k<N_local; k++) {
      for (l=0; l<N_local; l++) Z[k*N+l] = Z_local[k*N_local+l];
    }
  }

  /* Collect the result from the local matrices into a global matrix */
  if (grid_rank == 0) {
    /* Process zero receives the local matrices */
    for (i=1; i<nproc; i++) {    
      MPI_Recv(X_local, N_local*N_local, MPI_FLOAT, i, datatag, grid_comm,
	       &status);
      if (debug) {
	int limit;
	limit = min(dlimit, N_local);
	printf("\nThe %d*%d first entries in the result from process %d is\n",
	       limit,limit, i);
	fflush(stdout);
	write_matrix(X_local, limit);
      }

      /* Get the coordinates of process i */
      MPI_Cart_coords(grid_comm, i, 2, coordinates);
      /* Calculate index where to place the local matrix in the global result */
      startx = coordinates[0]*N_local;
      starty = coordinates[1]*N_local;

      /* Copy the result from process i into the global matrix Z */
      for (k=0; k<N_local; k++) {
	for (l=0; l<N_local; l++) {
	  Z[(startx+k)*N+(starty+l)] = X_local[k*N_local+l];
	}
      }
    }

    /* All other processes send their local matrices to process 0 */
  } else {
    MPI_Send(Z_local, N_local*N_local, MPI_FLOAT, 0, datatag, grid_comm);
  }


  /* Print the result of the matrix multiplication */
  if (debug && (id == 0)) {
    int limit;
    limit = min(dlimit, N);
    printf("The %d*%d first entries in the result matrix is\n", limit,limit);
    write_matrix(Z, limit);
    printf ("\n");
    fflush(stdout);
  }

  /* Write the result to a file */
  if (id == 0) {
    if (fwrite_matrix(Z, N, fn3)) {
      printf("Result of matrix multiplication written in file %s\n", fn3);
      fflush(stdout);
    }
    /* Free space for file names */
    free(fn1);
    free(fn2);
    free(fn3);
    /* Free space for matrices */
    free(X);
    free(Y);
    free(Z);
  }

  /* Free the local matrices */
  free(X_local);
  free(Y_local);
  free(Z_local);
  free(tmp);
  /* Free the created communicators */
  MPI_Comm_free(&grid_comm);
  MPI_Comm_free(&row_comm);
  MPI_Comm_free(&col_comm);
  
  MPI_Finalize();
  exit(0);
}
