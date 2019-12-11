#include <omp.h>
#include <stdio.h>
main()
{
  int x=0, y=0;
#pragma omp parallel shared(x) num_threads(6)
  {
    int t = omp_get_thread_num();
    #pragma omp critical 
    {
      x = x + t;
      y = y + 1;
    }
  } 
  printf("x = %d, y = %d\n",x,y);
}
