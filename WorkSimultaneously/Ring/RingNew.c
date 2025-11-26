#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>

#define K 1024       /* One Kilobyte */
#define M K *K       /* One Megabyte */
#define MAXSIZE K *M /* One Gigabyte */

int main(int argc, char *argv[])
{

  // Initialize MPI
  int np, id;
  int tag = 42;
  double starttime, endtime;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &np);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  /* Check that we run on more than one process */

  if (np < 2)
  {
    printf("You have to use at least 2 processes to run this program\n");
    MPI_Finalize(); // Quit if there is only one process
    exit(0);
  }

  /* Allocate a sufficiently large message buffer */
  char *buffer = (char *)malloc(MAXSIZE * sizeof(char));
  if (buffer == NULL)
  {
    MPI_Finalize();
    exit(0);
  }

  /* Check that allocation succeeded */
  if (buffer == NULL)
  {
    printf("Could not allocate memory, exiting\n");
    MPI_Finalize();
    exit(0);
  }

  // Initialize the whole message buffer to some values
  for (int i = 0; i < MAXSIZE; i++)
  {
    buffer[i] = (char)75;
  }

  int next = (id + 1) % np;
  int prev = (id - 1 + np) % np;
  long int inputSize;

  // Repeat until an inputSize <= 0 is entered
  if (id == 0)
  {

    // printf("Process %d prev is %d\n", id, prev);
    // printf("Process %d next is %d\n", id, next);
    // receive input from main process 0
    while (1)
    {
      printf("Please give an input size in bytes:\n");
      fflush(stdout);
      scanf("%d", &inputSize);

      if (inputSize > MAXSIZE)
      {
        printf("Input size is too large, maximum value is %d\n", MAXSIZE);
        inputSize = 0;
      }

      /* Send inputSize to all process */
      MPI_Bcast(&inputSize, 1, MPI_LONG, 0, MPI_COMM_WORLD);

      if (inputSize <= 0)
        break; /* Terminate if size = 0 */

      printf("Message size: %d\n", inputSize);
      fflush(stdout);

      /* Start measuring time */
      starttime = MPI_Wtime();

      // send inputSize to the next process
      MPI_Send(buffer, inputSize, MPI_CHAR, next, tag, MPI_COMM_WORLD);
      // printf("Process %d sending to process %d\n", id, next);
      MPI_Recv(buffer, inputSize, MPI_CHAR, prev, tag, MPI_COMM_WORLD, &status);
      // printf("Process %d received value %c from prev %d process %d\n", id, buffer, prev, status.MPI_SOURCE);
      // Stop measuring time
      endtime = MPI_Wtime();
      printf("Wall clock time = %f s\n", endtime - starttime);
      fflush(stdout);
    }
  }
  else
  {
    while (1)
    {
      // wait for inputSize from process 0
      MPI_Bcast(&inputSize, 1, MPI_LONG, 0, MPI_COMM_WORLD);
      if (inputSize <= 0)
      {
        break; /* Terminate if size = 0 */
      }
      for (int i = 0; i < np; i++)
      {
        if (id == i)
        {
          MPI_Recv(buffer, inputSize, MPI_CHAR, prev, tag, MPI_COMM_WORLD, &status);
          // printf("*Process %d received value %c from prev %d process %d\n", id, *buffer, prev, status.MPI_SOURCE);
          MPI_Send(buffer, inputSize, MPI_CHAR, next, tag, MPI_COMM_WORLD);
          // printf("*Process %d sending to process %d\n", id, next);
          fflush(stdout);
        }
      }
    }
  }

  free(buffer);   // Deallocate the buffer
  MPI_Finalize(); // Terminate MPI
  exit(0);        // Quit
}
