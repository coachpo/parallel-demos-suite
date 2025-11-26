/*
  OpenMP example program Hello World.
  The master thread forks a parallel region.
  All threads in the team obtain their thread number and print it.
  The master thread prints the total number of threads.
  Compile with: gcc -fopenmp omp_hello.c -o omp_hello
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
  
  int nthreads, tid;

  #ifdef _OPENMP
  printf("OpenMp is enabled\n\n");
  #endif
  
  /* Fork a team of threads giving them an own variable tid */  
#pragma omp parallel private(tid)
  {
    /* Get thread number */
    tid = omp_get_thread_num();

    /* Master thread sets the value of nthreads
       without syncronizing with the other threads */
    if (tid == 0) {
      nthreads = omp_get_num_threads();
      printf("Number of threads is %d\n", nthreads);
    }
    
    printf("Hello World from thread = %d of %d\n", tid, nthreads);
  }
  /* All threads join master thread and disband */
  exit(0);
}

