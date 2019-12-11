#include <omp.h>
#include <stdio.h>
int a, b, tid; 
#pragma omp threadprivate(a) 
main(int argc, char *argv[]) { 
    omp_set_dynamic(0); 
    printf("1st Parallel Region:\n"); 
#pragma omp parallel private(b,tid) 
{ 
    tid = omp_get_thread_num(); 
    a = tid; b = tid; 
    printf("Thread %d: a,b= %d %d\n",tid,a,b); 
} /* end of parallel region */ 
    printf("Master thread doing serial work here: a=%d\na,a");
    printf("2nd Parallel Region:\n"); 
#pragma omp parallel private(tid) 
{ 
    tid = omp_get_thread_num(); 
    printf("Thread %d: a,b= %d %d\n",tid,a,b); 
} /* end of parallel region */ 
} 
