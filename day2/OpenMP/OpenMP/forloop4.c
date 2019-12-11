#include <omp.h>
#include <stdio.h>
#define N 1000
main () { 
    int i, t;
    float a[N], b[N], c[N];
     for (i=0; i < N; i++) a[i] = b[i] = i * 1.0; 
    #pragma omp parallel shared(a,b,c) private(i,t) num_threads(6)
    {
        #pragma omp for schedule(runtime)
        for (i=0; i < N; i++){
             c[i] = a[i] + b[i];
             t = omp_get_thread_num();
             printf("Thread %d is doing iteration %d\n",t,i);
        }
    }
} 

