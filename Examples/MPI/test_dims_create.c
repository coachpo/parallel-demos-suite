/* 
   This program tests the behaviour of the MPI procedure MPI_Dims_create.
   It reads the total number of processes and the number of dimensions
   and prints out the result returned by MPI_Dims_create.

   Compile the program with 'mpicc test_dims_create.c -o test_dims_create'
   Run the program on one process. Terminate by entering a value of zero.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int id, ntasks;
  int nodes, dims;
  int *size;
  
  MPI_Init(&argc, &argv);	/* Initialize MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &ntasks);	/* Get nr of tasks */
  MPI_Comm_rank(MPI_COMM_WORLD, &id);	/* Get id of this process */

  /* Allocate space for the result, 10 is well enogh */
  size = (int *) malloc(10*sizeof(int));
  nodes = 1;

  while (nodes !=0) {

    printf("Nodes? "); fflush(stdout);
    scanf("%d", &nodes);
    if (nodes == 0) break;
    else {
      printf("Dims? "); fflush(stdout);
      scanf("%d", &dims);
      if (dims == 0) {
	printf("What ???\n");
	break;
      }
    }

    for (int i=0; i<dims; i++) size[i] = 0;

    MPI_Dims_create(nodes, dims, size);
    
    for (int i=0; i<dims; i++) {
      printf("dims[%d] = %d\n", i, size[i]);
    }
    printf("\n");

  }  /* End of while */

  MPI_Finalize();	         /* Terminate MPI */
  printf("Done\n");
  exit(0);
}
