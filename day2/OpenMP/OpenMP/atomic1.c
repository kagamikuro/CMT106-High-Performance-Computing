#include <omp.h>
#include <stdio.h>
main()
{
  int x=0;
#pragma omp parallel shared(x) num_threads(6)
  {
    int t = omp_get_thread_num();
    #pragma omp atomic
      x += t;
  } 
  printf("x = %d\n",x);
}
