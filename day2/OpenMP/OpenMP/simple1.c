#include <omp.h>
#include <stdio.h>
main () { 
#pragma omp parallel 
{
    int n=omp_get_num_threads();
    int tid=omp_get_thread_num();
    printf("There are %d threads. Hello from thread %d\n",n,tid);
}
/* end of parallel section */ 
    printf("Hello from the master thread\n");
} 
