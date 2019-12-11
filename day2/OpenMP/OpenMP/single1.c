#include <omp.h>
#include <stdio.h>
main () { 
#pragma omp parallel 
{
    int tid=omp_get_thread_num();
#pragma omp single
  {
    int n=omp_get_num_threads();
    printf("There are %d threads. This was output by thread %d\n",n,tid);
  }
  printf("Hello from thread %d\n",tid);
}
/* end of parallel section */ 
  printf("Hello from the master thread\n");
} 
