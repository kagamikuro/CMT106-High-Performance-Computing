#include <omp.h>
#include <stdio.h>
main () { 
#pragma omp parallel num_threads(6) 
{
#pragma omp single
{
#pragma omp taskgroup
{
#pragma omp task
{
    printf("Hello world!\n");
    #pragma omp task
    {
          sleep(1);
          printf ("Hello world from a nested task\n");
    }
}
} /* end of taskgroup */
#pragma omp task
{
    printf("Hello world again!\n");
}
}
} /* end of parallel section */ 
} 
