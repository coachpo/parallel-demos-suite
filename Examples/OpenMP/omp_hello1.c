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
  
  #ifdef _OPENMP
  printf("OpenMp is enabled\n\n");
  #endif
  
  /* Fork a team of threads and declare local variables */  
#pragma omp parallel
  {
    int nthreads, tid;    // Local variables are private to a thread

    /* Get thread number and number of threads */
    tid = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    
    /* Only master thread does this */
    if (tid == 0) {
      printf("Number of threads is %d\n", nthreads);
    }
    
    printf("Hello World from thread = %d\n", tid);
  }
  /* All threads join master thread and disband */
  exit(0);
}

