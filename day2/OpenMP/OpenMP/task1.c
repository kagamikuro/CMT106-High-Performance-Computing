#include <omp.h>
#include <stdio.h>
main () { 
#pragma omp parallel num_threads(6) 
{
#pragma omp task
{
    int tid=omp_get_thread_num();
    printf("Hello world from thread %d\n",tid);
}
} /* end of parallel section */ 
    printf("Hello from the master thread\n");
} 
