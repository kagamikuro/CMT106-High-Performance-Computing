#include <omp.h>
#include <stdio.h>
main () { 
    int i = 10;
#pragma omp parallel private(i) num_threads(4) 
{
    int x = 1;
    #pragma omp for
    for (i=0;i<100;i++) x++;
}
    printf("i = %d\n",i);
} 
