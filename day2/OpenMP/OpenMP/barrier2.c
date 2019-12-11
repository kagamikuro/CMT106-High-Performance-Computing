#include <omp.h>
#include <stdio.h>
main()
{
    int k, x[10], y=0, z=0;
#pragma omp parallel shared(x,y,z) private(k) num_threads(2)
{
    int t = omp_get_thread_num();
    if (t==0) y = 10;
    else z = 20;
    #pragma omp for
    for (k=0;k<10;k++) x[k] = y + z + k;
}
    printf("x = ");
    for(k=0;k<10;k++) printf("%d, ",x[k]);
    printf("\n");
}
