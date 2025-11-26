// gcc -O2 seq_sieve_parallel_improve.c -o seq_sieve_pi -lm -fopenmp -std=c99
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define DEBUG 0        /* Set to 1 if you want a lot of output */
#define POS(k) (k / 2) /* The position in the array of number k */
/* The number represented by position i */
unsigned long int nr(unsigned long int i)
{
  return 2 * i + 1;
}
void setBitTrue(char *arr, unsigned long int bit)
{
  unsigned long int index = bit / __CHAR_BIT__;
  int offset = bit % __CHAR_BIT__;
  char mask = (1 << offset);
#pragma omp atomic
  arr[index] |= mask;
}

void setBitFalse(char *arr, unsigned long int bit)
{
  unsigned long int index = bit / __CHAR_BIT__;
  int offset = bit % __CHAR_BIT__;
  char mask = (1 << offset);
#pragma omp atomic
  arr[index] &= ~mask;
}

// 查询第x位的值
int getBitValue(char *arr, unsigned long int bit)
{
  unsigned long int index = bit / __CHAR_BIT__;
  int offset = bit % __CHAR_BIT__;
  return (arr[index] >> offset) & 1;
}
int main(int argc, char *argv[])
{
  unsigned long int i, k, t, N, N_pos, nr_primes, lastprime = 0LL;
  char *prime = NULL;
  const char unmarked = (char)0;
  const char marked = (char)1;
  double start, stop;
  const int procs = omp_get_num_procs();

  if (argc < 2)
  {
    printf("Usage:  %s N\n", argv[0]);
    exit(-1);
  }
  N = atoll(argv[1]); // Get the value of N from the command line

  /* To store the marks from 3 to N we need (N-3)/2+1 positions */
  N_pos = (N - 3) / 2 + 1;

  start = omp_get_wtime(); /* Start measuring time */

  /* Allocate marks for all odd integers from 3 to N */
  prime = malloc(N_pos / __CHAR_BIT__ + 1); // Use one bit for each mark, plus one extra bit in case N_pos is odd
  if (prime == NULL)
  {
    printf("Could not allocate %lld chars of memory\n", N_pos);
    exit(-1);
  }

  /* Mark primes[0] since that is not used */
  // prime[0] = marked;
  setBitTrue(prime, 0);
  /* Initialize all odd numbers to unmarked */
  for (i = 1; i < N_pos; i++)
    // prime[i] = unmarked;
    setBitFalse(prime, i);

// put the for loop to different threads
#pragma omp parallel for private(k, t) shared(prime) schedule(dynamic)
  /* Position i in the array prime now corresponds to the number 2*i+1 */
  for (unsigned long int ii = 1; ii <= ((int)sqrt((double)N) - 1) / 2; ii++)
  {
    // if (prime[ii] == unmarked)
    if (getBitValue(prime, ii) == 0)
    { /* Next unmarked position */
      if (DEBUG)
        printf("Marking multiples of %lld: ", nr(ii));
      t = nr(ii); /* Position i corresponds to the number t */
      for (k = POS(t * t); k <= N_pos; k += t)
      {
        // prime[k] = marked; /* Mark the multiples of i */
        setBitTrue(prime, k);
        if (DEBUG)
          printf("%lld ", nr(k));
      }
      if (DEBUG)
        printf("\n");
    }
  }

  nr_primes = 1; /* Remember to count 2 as a prime */
/* Count the marked numbers */
#pragma omp parallel for reduction(+ : nr_primes) reduction(max : lastprime)
  for (unsigned long int ii = 1; ii <= N_pos; ii++)
  {
    // if (prime[ii] == unmarked)
    if (getBitValue(prime, ii) == 0)
    {
      lastprime = nr(ii);
      nr_primes++;
    }
  }

  stop = omp_get_wtime();
  printf("Time: %6.2f s\n", (float)(stop - start));

  if (DEBUG)
  {
    printf("\nPrime numbers smaller than or equal to %lld are\n", N);
    printf("%lld ", 2); /* Remember to print the value 2 */
    for (i = 1; i <= N_pos; i++)
    {
      if (prime[i] == unmarked)
      {
        printf("%lld ", nr(i));
      }
    }
  }

  printf("\n%d primes smaller than or equal to %lld\n", nr_primes, N);
  printf("The largest of these primes is %lld\n", lastprime);
  printf("\nReady\n");

  exit(0);
}
