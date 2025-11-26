#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

#define MAXPROC 8 /* Max number of procsses */

const double G = 6.67259e-7; /* Gravitational constant (should be e-10 but modified to get more action */
const double dt = 1.0;       /* Length of timestep */

/* Writes out positions (x,y) of N particles to the file fn
   Returns zero if the file couldn't be opened, otherwise 1 */
int write_particles(int N, double *X, double *Y, char *fn)
{
  FILE *fp;
  /* Open the file */
  if ((fp = fopen(fn, "w")) == NULL)
  {
    printf("Couldn't open file %s\n", fn);
    return 0;
  }
  /* Write the positions to the file fn */
  for (int i = 0; i < N; i++)
  {
    fprintf(fp, "%3.2f %3.2f \n", X[i], Y[i]);
  }
  fprintf(fp, "\n");
  fclose(fp); /* Close the file */
  return (1);
}

// Distance between points with coordinates (px,py) and (qx,qy)
double dist(double px, double py, double qx, double qy)
{
  return sqrt(pow(px - qx, 2) + pow(py - qy, 2));
  // Could also be written as sqrt( (px-qx)*(px-qx) + (py-qy)*(py-qy) )
}

/* Computes forces between bodies */
void ComputeForceParallel(int first, int last, int N, double *X, double *Y, double *mass, double *Fx, double *Fy)
{
  const double mindist = 0.0001; /* Minimal distance of two bodies of being in interaction*/

  for (int i = first; i < last; i++)
  {
    Fx[i - first] = Fy[i - first] = 0.0; // Initialize force vector to zero
    for (int j = 0; j < N; j++)
    {
      if (i != j)
      {
        // Distance between points i and j
        double r = dist(X[i], Y[i], X[j], Y[j]);

        if (r > mindist)
        {
          double r3 = pow(r, 3);
          Fx[i - first] += G * mass[i] * mass[j] * (X[j] - X[i]) / r3;
          Fy[i - first] += G * mass[i] * mass[j] * (Y[j] - Y[i]) / r3;
        }
      }
    }
  }
}

int main(int argc, char *argv[])
{
  int np, me;
  const int root = 0;                 /* Root process in scatter */
  MPI_Init(&argc, &argv);             /* Initialize MPI */
  MPI_Comm_size(MPI_COMM_WORLD, &np); /* Get nr of processes */
  MPI_Comm_rank(MPI_COMM_WORLD, &me); /* Get own identifier */
  double starttime, endtime;

  const int N = 1000;         // Number of bodies
  const int timeSteps = 1000; // Number of timeSteps
  const double size = 100.0;  // Initial positions are in the range [0,100]

  double *mass;  /* mass of bodies */
  double *X;     /* x-positions of bodies */
  double *Y;     /* y-positions of bodies */
  double *Vx;    /* velocities on x-axis of bodies */
  double *Vy;    /* velocities on y-axis of bodies */
  double *Fx;    /* forces on x-axis of bodies */
  double *Fy;    /* forces on y-axis of bodies */
  double *tempX; /* x-positions of bodies */
  double *tempY; /* y-positions of bodies */
  /* Allocate space for variables  */
  mass = (double *)calloc(N, sizeof(double)); // Mass
  X = (double *)calloc(N, sizeof(double));    // Position (x,y) at current time step
  Y = (double *)calloc(N, sizeof(double));
  Vx = (double *)calloc(N, sizeof(double)); // Velocities
  Vy = (double *)calloc(N, sizeof(double));
  Fx = (double *)calloc(N, sizeof(double)); // Forces
  Fy = (double *)calloc(N, sizeof(double));
  tempX = (double *)malloc(N / np * sizeof(double));
  tempY = (double *)malloc(N / np * sizeof(double));

  // Seed the random number generator so that it generates a fixed sequence
  unsigned short int seedval[3] = {7, 7, 7};
  seed48(seedval);

  // Use process 0 to generate oroginal data of mass and position arrays
  if (me == 0)
  {
    // Seed the random number generator so that it generates a fixed sequence
    unsigned short int seedval[3] = {7, 7, 7};
    seed48(seedval);
    /* Initialize mass and position of bodies */
    for (int i = 0; i < N; i++)
    {
      mass[i] = 1000.0 * drand48(); // 0 <= mass < 1000
      X[i] = size * drand48();      // 0 <= X < 100
      Y[i] = size * drand48();      // 0 <= Y < 100
    }
    // Write intial particle coordinates to a file
    write_particles(N, X, Y, "initial_pos_parallel.txt");
    // Start Timer
    starttime = MPI_Wtime();
  }

  /* Check that we have an even number of processes and at most MAXPROC */
  if (np > MAXPROC || np % 2 != 0)
  {
    if (me == 0)
    {
      printf("You have to use an even number of processes (at most %d)\n", MAXPROC);
    }
    MPI_Finalize();
    exit(0);
  }

  // Synchronize before Broadcasting
  MPI_Barrier(MPI_COMM_WORLD);
  // Broadcast the initial mass and position X and Y of bodies to each process other than 0
  MPI_Bcast(mass, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(X, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Bcast(Y, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  int first = N * me / np;
  int last = N * (me + 1) / np;
  int length = last - first;

  // Compute the initial forces that we get
  ComputeForceParallel(first, last, N, X, Y, mass, Fx, Fy);

  // Set up the velocity vectors caused by initial forces for Leapfrog method
  for (int i = 0; i < length; i++)
  {
    Vx[i] = 0.5 * dt * Fx[i] / mass[i + first];
    Vy[i] = 0.5 * dt * Fy[i] / mass[i + first];
  }

  /* Main loop:
    - Move the bodies
    - Calculate forces of the bodies with their new position
    - Calculate velocities of the bodies with the new forces
    - Copy the updated positions to the old positions (for use in next time step)
  */
  for (int t = 0; t < timeSteps; t++)
  {
    // Move the bodies
    for (int i = 0; i < length; i++)
    {
      tempX[i] = X[i + first] + Vx[i] * dt;
      tempY[i] = Y[i + first] + Vy[i] * dt;
    }

    if (me == 0)
    {
      printf("%d ", t);
      fflush(stdout); // Print out the timestep
    }
    // all processes gather the updated positions
    // need a machenisum to update local X and Y to Others and sycn in different Process.
    // MPI_Gather(tempX, length, MPI_DOUBLE, X, length, MPI_DOUBLE, 0,
    //            MPI_COMM_WORLD);
    // MPI_Gather(tempY, length, MPI_DOUBLE, Y, length, MPI_DOUBLE, 0,
    //            MPI_COMM_WORLD);

    // // Bcast X and Y in Process 0
    // MPI_Bcast(X, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // MPI_Bcast(Y, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Allgather(tempX, length, MPI_DOUBLE, X, length, MPI_DOUBLE, MPI_COMM_WORLD);
    MPI_Allgather(tempY, length, MPI_DOUBLE, Y, length, MPI_DOUBLE, MPI_COMM_WORLD);

    // Synchronize processes after updating X and Y Globally
    MPI_Barrier(MPI_COMM_WORLD);

    // calculates the forces between its own local bodies
    ComputeForceParallel(first, last, N, X, Y, mass, Fx, Fy);

    // Compute the velocities
    for (int i = 0; i < length; i++)
    {
      Vx[i] += dt * Fx[i] / mass[i + first];
      Vy[i] += dt * Fy[i] / mass[i + first];
    }
  }

  // Use Process 0 to print time and write final status to file.
  if (me == 0)
  {
    // End timer
    endtime = MPI_Wtime();

    printf("\n");
    printf("Time = %f s\n", endtime - starttime);
    write_particles(N, X, Y, "final_pos_parallel.txt");
  }

  // Clean up allocated memory
  free(X);
  free(Y);
  free(mass);
  free(tempX);
  free(tempY);
  free(Vx);
  free(Vy);
  free(Fx);
  free(Fy);
  MPI_Finalize();
  exit(0);
}
