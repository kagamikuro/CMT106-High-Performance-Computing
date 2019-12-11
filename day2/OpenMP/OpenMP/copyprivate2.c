#include <stdio.h>
main () { 
#pragma omp parallel num_threads(6)
{  
    int tid = omp_get_thread_num();
    int a = tid;  
    int x = 17;
#pragma omp single copyprivate(a,x)  
{  
    a = 10;
    x = 20;
}  
    printf("Thread %d: a=%d, x=%d\n",tid,a,x);
}
} 
