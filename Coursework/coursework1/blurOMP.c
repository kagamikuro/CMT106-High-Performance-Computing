#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>

#include <omp.h>

#define maxlen 200
#define rowsize 521
#define colsize 428
#define linelen 12



char str[maxlen], lines[5][maxlen];
FILE *fp, *fout;
int nlines = 0;
unsigned int h1, h2, h3;
char *sptr;
int R[rowsize][colsize], G[rowsize][colsize], B[rowsize][colsize];
int Rnew[rowsize][colsize], Gnew[rowsize][colsize], Bnew[rowsize][colsize];
int row = 0, col = 0, nblurs, lineno=0, k;
struct timeval tim;



void init(){
	

        fp = fopen("David.ps", "r");
        row = 0;
        while(! feof(fp))
        {
                fscanf(fp, "\n%[^\n]", str);
                if (nlines < 5) {strcpy((char *)lines[nlines++],(char *)str);}
                else{
                        for (sptr=&str[0];*sptr != '\0';sptr+=6){
                                sscanf(sptr,"%2x",&h1);
                                sscanf(sptr+2,"%2x",&h2);
                                sscanf(sptr+4,"%2x",&h3);

                                if (col==colsize){
                                        col = 0;
                                        row++;
                                }
                                if (row < rowsize) {
                                        R[row][col] = h1;
                                        G[row][col] = h2;
                                        B[row][col] = h3;
                                }
                                col++;
                        }
                }
        }
        fclose(fp);


}

void blur_OMP_Dynamic(int nthreads, int chunkSize){
        nblurs = 10;
        gettimeofday(&tim, NULL);
        double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
        
	
        for(k=0;k<nblurs;k++){
		#pragma omp parallel for collapse(2) shared(R,G,B,Rnew,Gnew,Bnew) private(row,col) num_threads(nthreads) schedule(dynamic,chunkSize) 
                for(row=0;row<rowsize;row++){
                        for (col=0;col<colsize;col++){
                                if (row != 0 && row != (rowsize-1) && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col+1]+R[row][col-1])/4;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col+1]+G[row][col-1])/4;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col+1]+B[row][col-1])/4;
                                }
                                else if (row == 0 && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row][col+1]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row][col+1]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row][col+1]+B[row][col-1])/3;
                                }
                                else if (row == (rowsize-1) && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row-1][col]+R[row][col+1]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row-1][col]+G[row][col+1]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row-1][col]+B[row][col+1]+B[row][col-1])/3;
                                }
                                else if (col == 0 && row != 0 && row != (rowsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col+1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col+1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col+1])/3;
                                }
                                else if (col == (colsize-1) && row != 0 && row != (rowsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col-1])/3;
                                }
                                else if (row==0 &&col==0){
                                        Rnew[row][col] = (R[row][col+1]+R[row+1][col])/2;
                                        Gnew[row][col] = (G[row][col+1]+G[row+1][col])/2;
                                        Bnew[row][col] = (B[row][col+1]+B[row+1][col])/2;
                                }
                                else if (row==0 &&col==(colsize-1)){
                                        Rnew[row][col] = (R[row][col-1]+R[row+1][col])/2;
                                        Gnew[row][col] = (G[row][col-1]+G[row+1][col])/2;
                                        Bnew[row][col] = (B[row][col-1]+B[row+1][col])/2;
                                }
                                else if (row==(rowsize-1) &&col==0){
                                        Rnew[row][col] = (R[row][col+1]+R[row-1][col])/2;
                                        Gnew[row][col] = (G[row][col+1]+G[row-1][col])/2;
                                        Bnew[row][col] = (B[row][col+1]+B[row-1][col])/2;
                                }
                                else if (row==(rowsize-1) &&col==(colsize-1)){
                                        Rnew[row][col] = (R[row][col-1]+R[row-1][col])/2;
                                        Gnew[row][col] = (G[row][col-1]+G[row-1][col])/2;
                                        Bnew[row][col] = (B[row][col-1]+B[row-1][col])/2;
                                }
                        }
                }
		for(row=0;row<rowsize;row++){
                        for (col=0;col<colsize;col++){
                            R[row][col] = Rnew[row][col];
                            G[row][col] = Gnew[row][col];
                            B[row][col] = Bnew[row][col];
                        }
                }
		
        }

       
        gettimeofday(&tim, NULL);
        double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
        printf("%.6lf seconds elapsed\n", t2-t1);

        fout= fopen("DavidBlur.ps", "w");
        for (k=0;k<nlines;k++) fprintf(fout,"\n%s", lines[k]);
        fprintf(fout,"\n");
        for(row=0;row<rowsize;row++){
                for (col=0;col<colsize;col++){
                        fprintf(fout,"%02x%02x%02x",R[row][col],G[row][col],B[row][col]);
                        lineno++;
                        if (lineno==linelen){
                                fprintf(fout,"\n");
                                lineno = 0;
                        }
                }
        }




}


void blur_OMP_Static(int nthreads, int chunkSize){
        nblurs = 10;
        gettimeofday(&tim, NULL);
        double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
        
	

        for(k=0;k<nblurs;k++){
		#pragma omp parallel for collapse(2) shared(R,G,B,Rnew,Gnew,Bnew) private(row,col) num_threads(nthreads) schedule(static,chunkSize) 
                for(row=0;row<rowsize;row++){
                        for (col=0;col<colsize;col++){
                                if (row != 0 && row != (rowsize-1) && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col+1]+R[row][col-1])/4;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col+1]+G[row][col-1])/4;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col+1]+B[row][col-1])/4;
                                }
                                else if (row == 0 && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row][col+1]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row][col+1]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row][col+1]+B[row][col-1])/3;
                                }
                                else if (row == (rowsize-1) && col != 0 && col != (colsize-1)){
                                        Rnew[row][col] = (R[row-1][col]+R[row][col+1]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row-1][col]+G[row][col+1]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row-1][col]+B[row][col+1]+B[row][col-1])/3;
                                }
                                else if (col == 0 && row != 0 && row != (rowsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col+1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col+1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col+1])/3;
                                }
                                else if (col == (colsize-1) && row != 0 && row != (rowsize-1)){
                                        Rnew[row][col] = (R[row+1][col]+R[row-1][col]+R[row][col-1])/3;
                                        Gnew[row][col] = (G[row+1][col]+G[row-1][col]+G[row][col-1])/3;
                                        Bnew[row][col] = (B[row+1][col]+B[row-1][col]+B[row][col-1])/3;
                                }
                                else if (row==0 &&col==0){
                                        Rnew[row][col] = (R[row][col+1]+R[row+1][col])/2;
                                        Gnew[row][col] = (G[row][col+1]+G[row+1][col])/2;
                                        Bnew[row][col] = (B[row][col+1]+B[row+1][col])/2;
                                }
                                else if (row==0 &&col==(colsize-1)){
                                        Rnew[row][col] = (R[row][col-1]+R[row+1][col])/2;
                                        Gnew[row][col] = (G[row][col-1]+G[row+1][col])/2;
                                        Bnew[row][col] = (B[row][col-1]+B[row+1][col])/2;
                                }
                                else if (row==(rowsize-1) &&col==0){
                                        Rnew[row][col] = (R[row][col+1]+R[row-1][col])/2;
                                        Gnew[row][col] = (G[row][col+1]+G[row-1][col])/2;
                                        Bnew[row][col] = (B[row][col+1]+B[row-1][col])/2;
                                }
                                else if (row==(rowsize-1) &&col==(colsize-1)){
                                        Rnew[row][col] = (R[row][col-1]+R[row-1][col])/2;
                                        Gnew[row][col] = (G[row][col-1]+G[row-1][col])/2;
                                        Bnew[row][col] = (B[row][col-1]+B[row-1][col])/2;
                                }
                        }
                }
		for(row=0;row<rowsize;row++){
                        for (col=0;col<colsize;col++){
                            R[row][col] = Rnew[row][col];
                            G[row][col] = Gnew[row][col];
                            B[row][col] = Bnew[row][col];
                        }
                }
		
        }

       
        gettimeofday(&tim, NULL);
        double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
        printf("%.6lf seconds elapsed\n", t2-t1);

        fout= fopen("DavidBlur.ps", "w");
        for (k=0;k<nlines;k++) fprintf(fout,"\n%s", lines[k]);
        fprintf(fout,"\n");
        for(row=0;row<rowsize;row++){
                for (col=0;col<colsize;col++){
                        fprintf(fout,"%02x%02x%02x",R[row][col],G[row][col],B[row][col]);
                        lineno++;
                        if (lineno==linelen){
                                fprintf(fout,"\n");
                                lineno = 0;
                        }
                }
        }




}







int main (int argc, const char * argv[]) {
    int nthreads = 4;
    int chunkSize = 1;
    init();

    //for dynamic scheduling
    printf("for dynamic scheduling\n");

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
    	printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Dynamic(nthreads,chunkSize);	

    }

    chunkSize = 100;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
        printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Dynamic(nthreads,chunkSize);
    }

    chunkSize = 10000;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
        printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
        init();
        blur_OMP_Dynamic(nthreads,chunkSize);
    }

    
    chunkSize = 100000;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
        printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Dynamic(nthreads,chunkSize);
    }


    

    
    //for static scheduling
    printf("for static scheduling\n");

    chunkSize = 1;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
	printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Static(nthreads,chunkSize);

    }

    chunkSize = 100;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
	printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Static(nthreads,chunkSize);

    }

    chunkSize = 10000;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
        printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
        init();
        blur_OMP_Dynamic(nthreads,chunkSize);
    }


    
    chunkSize = 100000;

    for(nthreads = 2;nthreads <= 16;nthreads *= 2){
        printf("for %d threads(chunk size = %d):\n",nthreads,chunkSize);
	init();
        blur_OMP_Static(nthreads,chunkSize);

    }
    


   		
    return 0;
}
