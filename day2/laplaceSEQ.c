#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#define DEFAULTNSTEPS 500
#define DEFAULTNPTSX 200
#define DEFAULTNPTSY 200

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

void performUpdates(float **phi, float **oldphi, int **mask, int nptsx, int nptsy, int nsteps)
{
   int k, j, i;
   struct timeval tim;
   double t1,t2;

   gettimeofday(&tim, NULL);
   t1=tim.tv_sec+(tim.tv_usec/1000000.0);
   for(k=1;k<=nsteps;k++){
         for(j=0;j<nptsy;j++)
            for(i=0;i<nptsx;i++){
               oldphi[j][i] = phi[j][i];
	    }
         for(j=0;j<nptsy;j++)
            for(i=0;i<nptsx;i++){
               if (mask[j][i]) phi[j][i] = 0.25*(oldphi[j][i+1] +
                  oldphi[j][i-1] + oldphi[j+1][i] + oldphi[j-1][i]);
	    }
   }
   gettimeofday(&tim, NULL);
   t2=tim.tv_sec+(tim.tv_usec/1000000.0);
   printf("laplaceSEQ: %.6lf seconds elapsed\n", t2-t1);
}

void setup_grid (float **phi, int nptsx, int nptsy, int **mask)
{
    int i, j, nx2, ny2;

    for(j=0;j<nptsy;j++)
       for(i=0;i<nptsx;i++){
          phi[j][i]  = 0.0;
          mask[j][i] = 1;
       }

    for(i=0;i<nptsx;i++) mask[0][i] = 0;

    for(i=0;i<nptsx;i++) mask[nptsy-1][i] = 0;

    for(j=0;j<nptsy;j++) mask[j][0] = 0;

    for(j=0;j<nptsy;j++) mask[j][nptsx-1] = 0;

    nx2 = nptsx/2;
    ny2 = nptsy/2;
    mask[ny2][nx2] = 0;
    mask[ny2][nx2-1] = 0;
    mask[ny2-1][nx2] = 0;
    mask[ny2-1][nx2-1] = 0;
    phi[ny2][nx2]  = 1.0;
    phi[ny2][nx2-1]  = 1.0;
    phi[ny2-1][nx2]  = 1.0;
    phi[ny2-1][nx2-1]  = 1.0;
}

int RGBval(float x){
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

void output_array (float **phi, int nptsx, int nptsy)
{
   int i, j, k=0;
   FILE *fp;

   
   int scalex = (nptsx < 500) ? nptsx : 500;
   int scaley = (nptsy < 300) ? nptsy : 300;
   int scale  = (scalex < scaley) ? scalex : scaley;
   fp = fopen("outSEQ.ps","w+");
   fprintf(fp,"/picstr %d string def\n",nptsx);
   fprintf(fp,"50 50 translate\n");
   fprintf(fp,"%d %d scale\n",scale, scale);
   fprintf(fp,"%d %d 8 [%d 0 0 %d 0 %d] \n",nptsx, nptsy, nptsx, nptsy, -nptsx);
   fprintf(fp,"{currentfile 3 200 mul string readhexstring pop} bind false 3 colorimage\n");

   for(j=0;j<nptsy;j++){
        for(i=0;i<nptsx;i++,k++){
             fprintf(fp,"%06x",RGBval(phi[j][i]));
             if((k+1)%10==0) fprintf(fp,"\n");
        }
   }
   fclose(fp);
}

void allocateArrays(float ***phi, float ***oldphi, int ***mask, int nptsx, int nptsy)
{
   int k;
   *phi = (float **)malloc(sizeof(float*)*nptsy);
   *oldphi = (float **)malloc(sizeof(float*)*nptsy);
   *mask = (int **)malloc(sizeof(int*)*nptsy);
   for (k=0;k<nptsy;k++){
        (*phi)[k] = (float *)malloc(sizeof(float)*nptsx);
        (*oldphi)[k] = (float *)malloc(sizeof(float)*nptsx);
        (*mask)[k] = (int *)malloc(sizeof(int)*nptsx);
   }
}

int main (int argc, char *argv[])
{
   float **phi, **oldphi;
   int   **mask;
   int nptsx, nptsy, nsteps;

   getParameters(argc, argv, &nptsx, &nptsy, &nsteps);
   allocateArrays(&phi, &oldphi, &mask, nptsx, nptsy);
   setup_grid (phi, nptsx, nptsy, mask);
   performUpdates(phi, oldphi, mask, nptsx, nptsy, nsteps);
   output_array (phi, nptsx, nptsy);
 
   return 0;
}
