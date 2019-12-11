#include <omp.h>
#include <stdio.h>
#define CHUNKSIZE 100
#define N 1000
main () { 
    int i, chunk, t;
    float a[N], b[N], c[N];
     for (i=0; i < N; i++) a[i] = b[i] = i * 1.0; 
    chunk = CHUNKSIZE;
    #pragma omp parallel shared(a,b,c,chunk) private(i,t) num_threads(6)
    {
        #pragma omp for schedule(guided,chunk)
        for (i=0; i < N; i++){
             c[i] = a[i] + b[i];
             t = omp_get_thread_num();
             printf("Thread %d is doing iteration %d\n",t,i);
        }
    }
} 

