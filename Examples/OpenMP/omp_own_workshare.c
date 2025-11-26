
/*
  OpenMP example of how you can use your own worksharing.

  Compile with gcc -O3 -fopenmp omp_own_workshare.c -o omp_own_workshare
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 25

int main (int argc, char *argv[]) {
  int X[N]; int Y[N];
  for (int i=0; i<N; i++) {     // Initialize X and Y
    X[i] = i;
    Y[i] = N-i;
  }
  
#pragma omp parallel shared(X,Y)
  {
    int tid = omp_get_thread_num();
    int nthreads = omp_get_num_threads();
    int chunk = N/nthreads;
    int start = tid*chunk;
    int end = (tid+1)*chunk-1;
    if (tid==nthreads-1) end = N-1;    // Give last iterations to the last thread

    printf("Thread %d has start=%d and end=%d\n", tid, start, end);
    
    for (int i=start; i<=end; i++) {
      X[i] += Y[i];
    }
  }

  // Print out the result
  for (int i=0; i<N; i++) {     // Initialize X and Y
    printf("%d  %d\n", i, X[i]);
  }
  

}


