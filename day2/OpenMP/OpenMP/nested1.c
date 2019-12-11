#include <omp.h>
#include <stdio.h>
#define CHUNKSIZE 20
#define N 10
main () { 
    int i, j, chunk, k[N][N];
    float a[N][N], b[N][N], c[N][N];
    for (i=0; i < N; i++)
         for (j=0;j<N; j++) {a[i][j] = (i*N+j) * 1.0; b[i][j] = (i+j*N)*1.0;}
    chunk = CHUNKSIZE;
    #pragma omp parallel shared(a,b,c,chunk) private(i,j) num_threads(5)
    {
        #pragma omp for schedule(static,chunk) nowait
            for (i=0; i < N; i++) 
                   for (j=0; j<N; j++){
                         c[i][j] = a[i][j] + b[i][j];
                         k[i][j] = omp_get_thread_num();
                  }
    }
     for (i=0; i < N; i++)
         for (j=0;j<N; j++)
              printf("%2d%c",k[i][j],(j==(N-1)? '\n' : ' '));
} 

