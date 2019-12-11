#include <omp.h>
#include <stdio.h>
main () { 
    int x = 10;
#pragma omp parallel firstprivate(x) num_threads(4) 
{
    int t  = omp_get_thread_num();
    int tx = x+t;
    printf("tx = %d on thread %d\n",tx,t);
}
} 
