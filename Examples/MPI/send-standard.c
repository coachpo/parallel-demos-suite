/*
A simple MPI example program using standard mode send and receive

The program consists of two processes. Process 0 sends a message
containing its identifier to the receiver. This receives the message
and sends it back.

Compile the program with 'mpicc send-standard.c -o send-standard'
*/

#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
  int np, me;
  int tag = 42;
  MPI_Status status;

  // We should of course check the result of these
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);

  /* Check that we run on exactly two processors */
  if (np != 2) {
    if (me == 0) {
      printf("You have to use exactly 2 processors to run this program\n");
    }
    MPI_Finalize();	       /* Quit if there is only one processor */
    exit(0);
  }
  
  int x = me;
  int y = -9999;
  if (me == 0) {   // Process 0

    printf("Process %d sending to process 1\n", me);    
    MPI_Send(&x, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);    // Send
    MPI_Recv (&y, 1, MPI_INT, 1, tag, MPI_COMM_WORLD, &status); // Receive
    printf ("Process %d received value %d from process %d\n", me, y, status.MPI_SOURCE);
    
  } else { // Process 1
    
    MPI_Send (&y, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
    MPI_Recv (&y, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
  }
  
  MPI_Finalize();
  exit(0);
}
