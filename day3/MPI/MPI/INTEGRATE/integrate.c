#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define PI 3.141592654
#define min(A,B) ((A)<(B) ? (A) : (B))

int main (int argc, char *argv[])
{
   int rank, nprocs;
   int icheck, i, nlocal, alpha, beta, nbeg, nend, m;
   double deltax, psum ,sum, x;

   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);

   if (rank==0) {
      	printf("\nGive the number of rectangles => \n");
      	icheck = scanf ("%d", &m);
   }

   MPI_Bcast (&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

   alpha = m/nprocs;
   beta = m%nprocs;
   nlocal = (rank<beta) ? alpha+1 : alpha;
   nbeg = (rank<beta) ? nlocal*rank : nlocal*rank + beta;
   nend = nbeg + nlocal - 1;

   deltax = PI/m;
   psum = 0.0;
   for(i=nbeg;i<=nend;i++){
      x = (i+0.5)*deltax;
      psum += sin(x);
   }

   MPI_Reduce (&psum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

   if (rank==0) printf ("\nThe integral is %f\n\n",sum*deltax);

   MPI_Finalize ();
   return 0;
}

