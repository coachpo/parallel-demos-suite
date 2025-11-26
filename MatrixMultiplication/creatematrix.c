/* Program that generates a square matrix of given size and stores it  */
/* in a file. The program asks for the dimension of the matrix, the    */
/* name of the file to store the matrix in and a random number seed.   */
 
/* Compile with  gcc -O3 creatematrix.c matrixutil.o -o creatematrix -lm  */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#include "matrixutil.h" 
//extern void write_matrix(float *M, int N);
//extern int fwrite_matrix(float *M, int N, char *fn);

int main(int argc, char** argv) {
 
  int debug = 0;       /* Debug flag, prints the d first entries of the matrices */
  int limit=10;
  int i, j;            /* Loop indexes */
  int seed;            /* Seed for random number generator */
  int N;               /* Size of matrix */
  float *X;            /* Created matrix */
  char *fn;
  char c;

  /* Parse arguments to see if we have a -v or -d flag */
  while ((c=getopt(argc, argv, "hd:")) != -1) {
    switch (c) {
    case 'd':
      debug = 1;              /* Set debug flag */
      limit = atoi(optarg);   /* Get the argument to -d  */
      break;
    case 'h':
      printf("Usage: creatematrix [-d N]\n");
      printf("       where\n");
      printf("        -d N   -- debug, print N by N first etries of the matrices\n");
      printf("        -h     -- help, print this message\n\n");
      exit(0);
    }
  }
  
  /* Allocate space for filename */
  fn = (char *) malloc(sizeof(char)*80);
  
  printf("Give dimension of matrix: ");
  scanf("%d",&N);
  if (N<limit) limit=N;   /* limit = min(N,limit) */

  printf("Give name of file to store matrix in: ");
  scanf("%s", fn);
 
  printf("Give random seed: ");
  scanf("%d",&seed);
  srand (seed);
 
  /* Allocate space for matrix */
  X = (float *) malloc(sizeof(float)*N*N);
 
  /* Assign random values to the matrix */
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      X[i*N+j] = (float)(rand()%10)-3.0;
    }
  }
  if (debug) {
    printf ("First %d by %d elements of the matrix is\n", limit, limit);
    write_matrix(X, limit);
  }
 
  if (fwrite_matrix (X, N, fn)) {
      printf("Wrote matrix to file %s\n\n", fn);
  }
 
  exit(0);
}
