/* Functions to read and write matrices in binary format.
   Compile with  gcc -O2 -c matrixutil.c   
*/

#include <stdio.h>

/* Reads a matrix M of size N*N from the file fn in binary format
   Returns zero if the file couldn't be opened, otherwise 1  */
int fread_matrix(float *M, int N, char *fn) {
  FILE *fp;
  int i;
  /* Open the file */
  if ((fp=fopen(fn, "r")) == NULL) {
    printf("Couldn't open file %s\n", fn);
    return(0);
  }
  /* Read the matrix from the file fn in binary format */
  fread(M, sizeof(float), N*N, fp);
  fclose (fp);
  return(1);
}  


/* Writes a matrix M of size N*N to the file fn in binary format
   Returns zero if the file couldn't be opened, otherwise 1       */
int fwrite_matrix(float *M, int N, char *fn) {
  FILE *fp;
  int i;
  /* Open the file */
  if ((fp=fopen(fn, "w")) == NULL) {
    printf("Couldn't open file %s\n", fn);
    return 0;
  }
  /* Write the matrix to the file fn in binary format */
  fwrite(M, sizeof(float), N*N, fp);
  fclose(fp);  /* Close the file */
  return(1);
}


/* Prints a matrix of size N*N  */
void write_matrix(float *M, int N) {
  int i, j, tmp;
  for (i=0; i<N; i++) {
    tmp = i*N;
    for (j=0; j<N; j++) {
      printf("%5.1f ", M[tmp+j]);
    }
    printf("\n");
  }
  printf("\n");
}


/* Multplies two square matrices X and Y of order N and places the
   result in Z. The matrix Z is assumed to be initialized to zero  */
void matrixmult(float *X, float *Y, float *Z, int N) {
  int i,j,k;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      for (k=0; k<N; k++) Z[i*N+j] += X[i*N+k]*Y[k*N+j];
    }
  }
}

/* Sets the elements of the square matrix X to zero */
void settozero(float *X, int N) {
  int i,j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) X[i*N+j] = 0.0;
  }
}

