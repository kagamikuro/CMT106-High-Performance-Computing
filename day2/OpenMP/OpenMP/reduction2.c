#include <omp.h>
#include <stdio.h>
main () { 
    int i, n, chunk;
    float a[100], result;
    n = 100;
    chunk = 10;
    for (i=0; i < n; i++) a[i] = i * 1.0;
    result = 0.0;
    #pragma omp parallel default(shared) private(i)
    #pragma omp for schedule(static,chunk) reduction(+:result)
    for (i=0; i < n; i++) result = result + a[i] ; 
    printf("Average = %f\n",result/(float)n);
} 

