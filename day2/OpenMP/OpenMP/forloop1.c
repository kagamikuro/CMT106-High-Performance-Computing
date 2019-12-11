#include <omp.h>
#include <stdio.h>
#define CHUNKSIZE 100
#define N 1000
main () { 
    int i, chunk, t, n;
    float a[N], b[N], c[N];
     for (i=0; i < N; i++) a[i] = b[i] = i * 1.0; 
    chunk = CHUNKSIZE;
    #pragma omp parallel shared(a,b,c,chunk) private(i,n,t) num_threads(6)
    {
        #pragma omp for schedule(static,chunk)
        for (i=0; i < N; i++){
             c[i] = a[i] + b[i];
             n = i/chunk;
             t = omp_get_thread_num();
             if(i%chunk==0) printf("Thread %d is doing chunk %d\n",t,n);
        }
    }
/* end of parallel section */ 
} 

