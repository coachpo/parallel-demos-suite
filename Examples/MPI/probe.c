/************************************************************************
Example MPI program that uses MPI_Probe to test if a message has arrived.

Compile the program with 'mpicc probe.c -o probe'
Run it with: srun -n 2 probe
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "mpi.h"

#define MAX_NUMBERS 100

int main(int argc, char* argv[]) {
  int np, me;
  const int tag  = 42;                  /* Tag value for communication */
  const int maxValue = 1000;
  
  MPI_Init(&argc, &argv);                /* Initialize MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &np);    /* Get nr of processes */
  MPI_Comm_rank(MPI_COMM_WORLD, &me);    /* Get own identifier */
  
  /* Check that we run on two processes */
  if (np!=2 ) {
    if (me == 0) {
      printf("You have to run the program on two processes\n");
    }
    MPI_Finalize();
    exit(0);
  }

  if (me == 0) {
    int numbers[MAX_NUMBERS];
    
    // Draw a random number in [0, MAX_NUMBERS]
    srandom(time(NULL));                         // Set seed
    int howMany = (int) random()%MAX_NUMBERS;   // Random number between 0 and 99
    
    for (int i=0; i<howMany; i++) {            // Draw random values in interval [0,999]
      numbers[i] = (int) random()%maxValue;
    }

    // Send the amount of integers to the other process
    MPI_Send(numbers, howMany, MPI_INT, 1, tag, MPI_COMM_WORLD);
    printf("Process 0 sent %d numbers \n", howMany); 
    fflush(stdout);
    
  } else if (me == 1) {
    MPI_Status status;
    int n;

    // Probe for an incoming message from process zero
    MPI_Probe(0, tag, MPI_COMM_WORLD, &status);

    // When probe returns, the status object has the size and other
    // attributes of the incoming message. Now get the message size.
    MPI_Get_count(&status, MPI_INT, &n);
    
    // Allocate a buffer to hold the incoming numbers
    int* receive_buf = (int*)malloc(sizeof(int) * n);
    if (receive_buf == NULL) {
	printf("Could not allocate memory\n");
	exit(-1);
      }
    
    // Now receive the message into the allocated buffer
    MPI_Recv(receive_buf, n, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Process 1 received %d numbers from process 0.\n", n); 
    for (int i = 0; i<n; i++) {
      printf("%d ", receive_buf[i]);
    }
    printf("\n");
    fflush(stdout);
    
    free(receive_buf);
  }

  MPI_Finalize();
  exit(0);
}


  
