#include <omp.h>
#include <stdio.h>
int a, tid; 
#pragma omp threadprivate(a) 
main(int argc, char *argv[]) { 
    omp_set_dynamic(0); 
    a = 10;
    printf("1st Parallel Region:\n"); 
#pragma omp parallel private(tid) copyin(a) 
{ 
    tid = omp_get_thread_num(); 
    a += tid;
    printf("Thread %d: a = %d\n",tid,a); 
} /* end of parallel region */ 
    printf("Master thread doing serial work: a=%d\n",a);
    printf("2nd Parallel Region:\n"); 
#pragma omp parallel private(tid) copyin(a)
{ 
    tid = omp_get_thread_num(); 
    printf("Thread %d: a= %d\n",tid,a); 
} /* end of parallel region */ 
} 
