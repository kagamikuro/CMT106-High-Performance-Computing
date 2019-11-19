#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h>

#define DEFAULTNPTSX 200
#define DEFAULTNPTSY 200
#define DEFAULTNSTEPS 500

void allocateArrays(double ***phi, double ***oldphi, int ***mask, int nsizex, int nsizey)
{
   int k;
   *phi = (double **)malloc(sizeof(double*)*(nsizey+2));
   *oldphi = (double **)malloc(sizeof(double*)*(nsizey+2));
   *mask = (int **)malloc(sizeof(int*)*(nsizey+2));
   for (k=0;k<nptsy;k++){
        (*phi)[k] = (double *)malloc(sizeof(double)*(nsizex+2));
        (*oldphi)[k] = (double *)malloc(sizeof(double)*(nsizex+2));
        (*mask)[k] = (int *)malloc(sizeof(int)*(nsizex+2));
   }
}

void getParameters(int argc, char *argv[], int *nptsx, int *nptsy, int *nsteps)
{
   int i;
   *nptsx = DEFAULTNPTSX; 
   *nptsy = DEFAULTNPTSY;
   *nsteps  = DEFAULTNSTEPS;

   for(i = 0; i < argc; i++) {
    if((strcmp(argv[i], "-nx") == 0) || (strcmp(argv[i], "--nptsx") == 0)) {
      *nptsx = atoi(argv[++i]);
      continue;
    }
    if((strcmp(argv[i], "-ny") == 0) || (strcmp(argv[i], "--nptsy") == 0)) {
      *nptsy = atoi(argv[++i]);
      continue;
    }
    if((strcmp(argv[i], "-ns") == 0) || (strcmp(argv[i], "--nsteps") == 0)) {
      *nsteps = atoi(argv[++i]);
      continue;
    }
   }
}

int RGBval(double x){
    int R, B, G, pow8 = 256;
    x=pow(x,0.33333);
    if(x<=0.5){
        B = (int)((1.0-2.0*x)*255.0);
        G = (int)(2.0*x*255.0);
	R = 0; 
    }
    else{
        B = 0;
        G = (int)((2.0-2.0*x)*255.0);
        R = (int)((2.0*x-1.0)*255.0);
    }
    return (B+(G+R*pow8)*pow8);
}

void setup_grid (phi, nptsx, nptsy, nprocx, nprocy, myposx, myposy, nlocalx, nlocaly, mask)
double  **phi;
int     nptsx, nptsy, nprocx, nprocy, myposx, myposy, nlocalx, nlocaly;
int     **mask;
{
    int i, j, global_x, global_y;

    for(j=0;j<=nlocaly+1;j++)
       for(i=0;i<=nlocalx+1;i++){
          phi[j][i]  = 0.0;
          mask[j][i] = 1;
       }

    if (myposy == 0)
       for(i=0;i<=nlocalx+1;i++) mask[1][i] = 0;

    if (myposy == nprocy-1)
       for(i=0;i<=nlocalx+1;i++) mask[nlocaly][i] = 0;

    if (myposx == 0)
       for(j=0;j<=nlocaly+1;j++) mask[j][1] = 0;

    if (myposx == nprocx-1)
       for(j=0;j<=nlocaly+1;j++) mask[j][nlocalx] = 0;

    for(j=1;j<=nlocaly;j++){
       global_y = nlocaly*myposy + j - 1;
       if (global_y == nptsy/2 || global_y == nptsy/2-1){
          for(i=1;i<=nlocalx;i++){
             global_x = nlocalx*myposx + i - 1;
             if (global_x == nptsx/2 || global_x == nptsx/2-1){
                mask[j][i] = 0;
                phi[j][i]  = 1.0;
             }
          }
       }
    }
}

void output_array (phi, rank, nprocx, nprocy, nlocalx, nlocaly, nprocs, myposx, myposy, nptsx, nptsy, new_comm)
double **phi;
int    rank, nprocx, nprocy, nlocalx, nlocaly, nprocs, myposx, myposy, nptsx, nptsy;
MPI_Comm new_comm;
{
   int i, j, k=0, m, n;
   int jmax, nsizey, count, source;
   int coords[2];
   MPI_Status status;
   FILE *fp;

   
   if(rank==0){
	 fp = fopen("outMPI.ps","w+");
         fprintf(fp,"/picstr %d string def\n",nptsx);
         fprintf(fp,"50 50 translate\n");

         int scalex = (nptsx < 500) ? nptsx : 500;
         int scaley = (nptsy < 300) ? nptsy : 300;
         int scale  = (scalex < scaley) ? scalex : scaley;
         fprintf(fp,"%d %d scale\n",scale, scale);
//         fprintf(fp,"%d %d scale\n",nptsx, nptsy);
         fprintf(fp,"%d %d 8 [%d 0 0 %d 0 %d] \n",nptsx, nptsy, nptsx, nptsy, -nptsx);
         fprintf(fp,"{currentfile 3 200 mul string readhexstring pop} bind false 3 colorimage\n");
   }

   nsizey = (nptsy-1)/nprocy + 1;
   for(m=0;m<nprocy;m++){
      jmax = (m==nprocy-1) ? nptsy - nsizey*(nprocy-1) : nsizey;
      for(j=1;j<=jmax;j++){
         for(n=0;n<nprocx;n++){
            if (myposx == n && myposy == m)
               if (rank!=0){
                      MPI_Send (&phi[j][1], nlocalx, MPI_DOUBLE, 0, 115,MPI_COMM_WORLD);
               }
               else
                  for(i=1;i<=nlocalx;i++,k++){
                     fprintf(fp,"%06x",RGBval(phi[j][i]));
                     if((k+1)%10==0) fprintf(fp,"\n");
//                     printf("%9.6f%c",phi[j][i],((k+1)%10==0) ? '\n' : ' ');
                  }
            else if (rank==0){
               coords[0] = m;
               coords[1] = n;
               MPI_Cart_rank (new_comm, coords, &source);
               MPI_Recv (&phi[0][1], nlocalx, MPI_DOUBLE, source, 115, MPI_COMM_WORLD, &status);
               MPI_Get_count(&status, MPI_DOUBLE, &count);
// need to check number received here
               for(i=1;i<=count;i++,k++){
                     fprintf(fp,"%06x",RGBval(phi[0][i]));
                     if((k+1)%10==0) fprintf(fp,"\n");
//                  printf("%9.6f%c",phi[1][i],((k+1)%10==0) ? '\n' : ' ');
               }
            }
         }
     }
   }
    if(rank==0) fclose(fp);
}

int main (int argc, char *argv[])
{
   double *sbuf, *rbuf;
   double **phi, **oldphi;
   int    **mask;
   int    i, j, k;
   int    rank, nprocs, dims[2], periods[2], coords[2];
   int    myposx, myposy, nprocx, nprocy;
   int    up, down, left, right;
   int    bufsize, nsizex, nsizey, nlocalx, nlocaly;
   int    nptsx, nptsy, nsteps;
   struct timeval tim;
   double t1,t2;
   MPI_Comm new_comm;
   MPI_Status status;

   getParameters(argc, argv, &nptsx, &nptsy, &nsteps);

/* Initialise and find rank and number of processes */
   MPI_Init (&argc, &argv);
   MPI_Comm_size (MPI_COMM_WORLD, &nprocs);
   MPI_Comm_rank (MPI_COMM_WORLD, &rank);

/* Work out number of processes in each direction of the process mesh */
   dims[0] = dims[1] = 0;
   MPI_Dims_create (nprocs, 2, dims);
   nprocy = dims[0];
   nprocx = dims[1];

   if (nptsx%nprocx!=0 || nptsy%nprocy!=0) {
	   if(rank==0) printf("Sorry! Number of points must be exactly divisible by the number of processes in that direction\nnptsx=%d, nptsy=%d, nprocx=%d, nprocy=%d\n",nptsx,nptsy,nprocx,nprocy);
	   MPI_Finalize();
	   exit(0);
   }


// INSERT CODE 1:
// 	Here you must insert 2 calls:
// 		1. Call MPI_Cart_create to create a 2D communicator called new_comm. You
// 		   should use the arrays dims and periods, which have already been set up.
// 		2. Call MPI_Cart_coords to return the 2D array coords, which contains the
// 		   number of proceesors in each direction. You should use new_comm and rank.


   myposy = coords[0];
   myposx = coords[1];

// INSERT CODE 2:
// 	Here you must insert 2 calls to MPI_Cart_shift:
// 		1. To find the neighboring processors in the up/down direction.
// 		2. To find the neighboring processors in the left/right direction.
   
   


// Allocate arrays
   nsizex = (nptsx-1)/nprocx + 1;
   nsizey = (nptsy-1)/nprocy + 1;
   bufsize = (nsizex>nsizey) ? nsizex : nsizey;
   sbuf = (double *)malloc((sizeof(double)*bufsize));
   rbuf = (double *)malloc((sizeof(double)*bufsize));

   allocateArrays(&phi, &oldphi, &mask, nsizex, nsizey);

   nlocalx = (myposx==nprocx-1) ? nptsx-nsizex*(nprocx-1) : nsizex;
   nlocaly = (myposy==nprocy-1) ? nptsy-nsizey*(nprocy-1) : nsizey;

   printf("rank = %d: (myposx,myposy)=(%d,%d), (left,right,down,up)=(%d,%d,%d,%d), (nlocalx,nlocaly)=(%d,%d)\n",rank,myposx,myposy,left,right,down,up,nlocalx,nlocaly);

/* Initialise arrays */
   setup_grid (phi, nptsx, nptsy, nprocx, nprocy, myposx, myposy, nlocalx, nlocaly, mask);
 
   MPI_Barrier(new_comm);
   if(rank==0){
        gettimeofday(&tim, NULL);
        t1=tim.tv_sec+(tim.tv_usec/1000000.0);
   }
/* Iterate to find solution */
   for(k=1;k<=nsteps;k++){
         for(j=1;j<=nlocaly;j++)
            for(i=1;i<=nlocalx;i++)
               oldphi[j][i] = phi[j][i];
// INSERT CODE 3:
// 	Here you must insert 2 calls to MPI_Sendrecv:
// 		1. Send row nlocaly of oldphi to the process above, while receiving data from 
// 		   the process below which is stored in row 0 of oldphi
// 		2. Send row 1 of oldphi to the process below, while receiving data from the
// 		   process above which is stored in row (nlocaly+1) of oldphi

	
// INSERT CODE 4:
// 	Here you must do the communication in the left/right direction:
// 		1. Copy column nlocalx of oldphi into the array sbuf.
// 		2. Send sbuf to the process on the right, while receiving data from the
// 		   process on the left into the array rbuf. You should use MPI_Sendrecv
// 		   to do this communiction.
// 		3. Copy rbuf into column 0 of oldphi.
// 		4. Copy column 1 of oldphi into the array sbuf.
// 		5. Send sbuf to the process on the left, while receiving data from the
// 		   process on the right into the array rbuf. You should use MPI_Sendrecv 
// 		   to do this communiction.
// 		6. Copy rbuf into column (nlocalx+1) of oldphi.
	
	 

         for(j=1;j<=nlocaly;j++)
            for(i=1;i<=nlocalx;i++)
               if (mask[j][i]) phi[j][i] = 0.25*(oldphi[j][i-1] +
                  oldphi[j][i+1] + oldphi[j-1][i] + oldphi[j+1][i]);
   }
   MPI_Barrier(new_comm);
   if(rank==0){
       gettimeofday(&tim, NULL);
       t2=tim.tv_sec+(tim.tv_usec/1000000.0);
       printf("laplaceMPI: %.6lf seconds elapsed\n", t2-t1);
   }
   output_array (phi, rank, nprocx, nprocy, nlocalx, nlocaly, nprocs , myposx, myposy, nptsx, nptsy, new_comm);
 
   MPI_Finalize();
   return 0;
}
