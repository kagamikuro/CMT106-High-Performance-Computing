#include <omp.h>
#include <stdio.h>
#define N 1000
main ()
{
    int i, t;
    float a[N], b[N], c[N], d[N];
    for (i=0; i < N; i++) {
        a[i] = i * 1.5;
        b[i] = i + 22.35;
     }
#pragma omp parallel shared(a,b,c,d) private(i,t) num_threads(2)
  {
    t = omp_get_thread_num();
  #pragma omp sections
    {
    #pragma omp section
      {
        printf("Thread %d is doing the first section\n",t);
        for (i=0; i < N; i++) c[i] = a[i] + b[i];
      }
    #pragma omp section
      {
        printf("Thread %d is doing the second section\n",t);
        for (i=0; i < N; i++) d[i] = a[i] * b[i];
      }
    }  /* end of sections */
  }  /* end of parallel section */
}
