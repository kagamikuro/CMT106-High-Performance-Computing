#include <stdio.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
   int rank, n, i, message, tag=111;
   MPI_Status status;

   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &n);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);

   if (rank==0) {  /* Process 0 will output data */
      printf ("Hello from process %3d\n", rank);
      for (i=1;i<n;i++) {
         MPI_Recv (&message,1,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG, 				  MPI_COMM_WORLD,&status);
         printf ("Hello from process %3d\n", message);
      }
   }
   else MPI_Send (&rank, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);

   MPI_Finalize ();
   return 0;
}
