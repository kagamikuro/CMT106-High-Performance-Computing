#include <omp.h>
#include <stdio.h>
main () { 
    int i, n, chunk, result3=0;
    float a[100], b[100], result1, result2;
    n = 100;
    chunk = 10;
    result1 = result2 = 0.0;
    for (i=0; i < n; i++) {
        a[i] = i * 1.0;
        b[i] = i * 2.0;
    }
    #pragma omp parallel for default(shared) private(i) schedule(static,chunk) \
            	reduction(+:result1, result2)  reduction (|:result3)
    for (i=0; i < n; i++) {
        result1 = result1 + a[i] ; 
        result2 = result2 + b[i];
        result3 = result3|i;
   }
    printf("Finally, result1 = %f, result2 = %f, and result3 = %d\n",result1,result2,result3);
} 
