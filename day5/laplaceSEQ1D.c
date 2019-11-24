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

void performUpdates(float *h_phi, float *h_oldphi, int *h_mask, int nptsx, int nptsy, int nsteps)
{
   int k, j, i, x, ijm, ijp;
   struct timeval tim;
   double t1,t2;

   gettimeofday(&tim, NULL);
   t1=tim.tv_sec+(tim.tv_usec/1000000.0);
   for(k=1;k<=nsteps;k++){
         for(j=0;j<nptsy;j++)
            for(i=0;i<nptsx;i++){
	       x = j*nptsx + i;
               h_oldphi[x] = h_phi[x];
	    }
         for(j=0;j<nptsy;j++)
            for(i=0;i<nptsx;i++){
	       x = j*nptsx + i;
	       ijp = (j+1)*nptsx + i;
	       ijm = (j-1)*nptsx + i;
               if (h_mask[x]) h_phi[x] = 0.25*(h_oldphi[x+1] +
                  h_oldphi[x-1] + h_oldphi[x+nptsx] + h_oldphi[x-nptsx]);
	    }
   }
   gettimeofday(&tim, NULL);
   t2=tim.tv_sec+(tim.tv_usec/1000000.0);
   printf("laplaceSEQ1D: %.6lf seconds elapsed\n", t2-t1);
}

void setup_grid (float *h_phi, int nptsx, int nptsy, int *h_mask)
{
    int i, j, ij, nx2, ny2;

    for(j=0;j<nptsy;j++)
       for(i=0;i<nptsx;i++){
	  ij = j*nptsx + i;
          h_phi[ij]  = 0.0;
          h_mask[ij] = 1;
       }

    for(i=0;i<nptsx;i++) h_mask[i] = 0;

    for(i=0;i<nptsx;i++) h_mask[nptsx*(nptsy-1)+i] = 0;

    for(j=0;j<nptsy;j++) h_mask[nptsx*j] = 0;

    for(j=0;j<nptsy;j++) h_mask[nptsx*j+nptsx-1] = 0;

    nx2 = nptsx/2;
    ny2 = nptsy/2;
    h_mask[nptsx*ny2+nx2] = 0;
    h_mask[nptsx*ny2+nx2-1] = 0;
    h_mask[nptsx*(ny2-1)+nx2] = 0;
    h_mask[nptsx*(ny2-1)+nx2-1] = 0;
    h_phi[nptsx*ny2+nx2]  = 1.0;
    h_phi[nptsx*ny2+nx2-1]  = 1.0;
    h_phi[nptsx*(ny2-1)+nx2]  = 1.0;
    h_phi[nptsx*(ny2-1)+nx2-1]  = 1.0;
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

void output_array (float *h_phi, int nptsx, int nptsy)
{
   int i, j, k=0;
   FILE *fp;

   
   int scalex = (nptsx < 500) ? nptsx : 500;
   int scaley = (nptsy < 300) ? nptsy : 300;
   int scale  = (scalex < scaley) ? scalex : scaley;
   fp = fopen("outSEQ1D.ps","w+");
   fprintf(fp,"/picstr %d string def\n",nptsx);
   fprintf(fp,"50 50 translate\n");
   fprintf(fp,"%d %d scale\n",scale, scale);
   fprintf(fp,"%d %d 8 [%d 0 0 %d 0 %d] \n",nptsx, nptsy, nptsx, nptsy, -nptsx);
   fprintf(fp,"{currentfile 3 200 mul string readhexstring pop} bind false 3 colorimage\n");

   for(j=0;j<nptsy;j++){
        for(i=0;i<nptsx;i++,k++){
             fprintf(fp,"%06x",RGBval(h_phi[nptsx*j+i]));
             if((k+1)%10==0) fprintf(fp,"\n");
        }
   }
   fclose(fp);
}

int main (int argc, char *argv[])
{
   float *h_phi, *h_oldphi;
   int    *h_mask;
   int nptsx, nptsy, nsteps;

   getParameters(argc, argv, &nptsx, &nptsy, &nsteps);
   int nsize1 = sizeof(float)*nptsx*nptsy;
   int nsize2 = sizeof(int)*nptsx*nptsy;
   h_phi = (float *)malloc(nsize1);
   h_oldphi = (float *)malloc(nsize1);
   h_mask = (int *)malloc(nsize2);
   setup_grid (h_phi, nptsx, nptsy, h_mask);
   performUpdates(h_phi, h_oldphi, h_mask, nptsx, nptsy, nsteps);
   output_array (h_phi, nptsx, nptsy);
 
   return 0;
}
