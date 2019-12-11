#include <omp.h>
#include <stdio.h>
main () { 
#pragma omp parallel num_threads(6) 
{
#pragma omp single
{
#pragma omp task
{
    printf("Hello world!\n");
}
#pragma omp task
{
    printf("Hello world again!\n");
}
}
} /* end of parallel section */ 
} 
