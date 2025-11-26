
extern void write_matrix(float *M, int N);
extern int  fread_matrix(float *M, int N, char *fn);
extern int  fwrite_matrix(float *M, int N, char *fn);
extern void matrixmult(float *X, float *Y, float *Z, int N);
extern void matrixmult_block(float *X, float *Y, float *Z, int N, int blocksize);
extern void matrixmult_slice(float *X, float *Y, float *Z, int N, int blocksize);
extern void settozero(float *X, int N);
