#include <stdio.h>
#include <mpi.h>

int main (int argc, char *argv[])
{
   int rank, n, i, message, plen;
   char pname[MPI_MAX_PROCESSOR_NAME+1];
   MPI_Status status;

   MPI_Init (&argc, &argv);

   MPI_Comm_size (MPI_COMM_WORLD, &n);

   MPI_Comm_rank (MPI_COMM_WORLD, &rank);

   MPI_Get_processor_name(pname,&plen);

   if (rank==0) printf("Number of processes is %d\n",n);
   printf("Process %d is on machine %s\n",rank,pname);

   MPI_Finalize ();
   return 0;
}
