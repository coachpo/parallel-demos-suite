/*
  MPI example program that computes the value of Pi using a Monte Carlo
  simulation. The program samples points inside the rectangle delimited
  by (0,0) and (1,1) and counts how many of these are within a circle
  with a radius of 1. The ratio between the number of points inside the
  circle and the total number of samples is Pi/4. All processes use
  different random number sequences.

  Compile with: mpicc -O3 cpi_mc.c -lm -o cpi_mc
  Run with: srun -n 4 cpi_mc
*/

#include "mpi.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc,char *argv[])
{
  const double PI24 = 3.141592653589793238462643;
  int myid, numprocs;
  double starttime, endtime;
  
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myid);
  
  // Seed the random number generator in each process
  srand48(myid*time(NULL));

  long int n;
  /* Read the number of random samples  */
  if (myid == 0) {
    printf("Give number of samples in each process: \n"); fflush(stdout);
    scanf("%ld", &n);
  }
  /* Send n to all proceses */
  MPI_Bcast(&n, 1, MPI_LONG, 0, MPI_COMM_WORLD);
  if (myid == 0) 
    printf("Number of samples is %ld\n", n);
  fflush(stdout);
  
  /* Start measuring time */
  if (myid == 0) {
    starttime = MPI_Wtime();
  }

  double in_circle = 0.0;
  /* Draw n random points and count how many are inside the circle */
  for (long int i=0; i<n; i++) {
    ////double x, y, dist;
    double x = drand48(); /* Draw a random point */
    double y = drand48();
    double dist = sqrt(x*x+y*y);        /* Compute distance of the point (x,y) to origo */
    if (dist<=1.0) in_circle++;  /* Count how many points are inside the circle */
  }

  double sum_c = 0.0;
  /* Sum all the samples inside the circle */
  MPI_Reduce(&in_circle, &sum_c, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  if (myid == 0) {
    endtime = MPI_Wtime();          // Stop measuring time
    double sum_n = (double) (n*numprocs);  /* Total nr of random points */
    double pi = (sum_c/sum_n)*4.0;
    printf("The computed value of Pi is %2.24f\n", pi);
    printf("The  \"exact\" value of Pi is %2.24f\n", PI24);
    printf("The difference is %e\n", fabs(PI24-pi));
    printf("Wall clock time = %f s\n", endtime-starttime);	       
    fflush(stdout);
  }
  
  MPI_Finalize();
  exit(0);
}
