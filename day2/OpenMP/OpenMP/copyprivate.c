#include <stdio.h>
int x;  
#pragma omp threadprivate(x)  
main () { 
#pragma omp parallel num_threads(6)
{  
    int tid = omp_get_thread_num();
    int a = tid;  
#pragma omp single copyprivate(x)  
{  
    a = 10;
    x = 20;
}  
    printf("Thread %d: a=%d, x=%d\n",tid,a,x);
}
} 
