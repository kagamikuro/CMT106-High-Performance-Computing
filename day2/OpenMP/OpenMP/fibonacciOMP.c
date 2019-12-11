#include <omp.h>
#include <stdio.h>
int fib(int n)
{
  int i, j;
  if (n<2)
      return n;
  else {
#pragma omp task shared(i) firstprivate(n)
      i=fib(n-1);
#pragma omp task shared(j) firstprivate(n)
      j=fib(n-2);
#pragma omp taskwait
      return i+j;
   }
}

int main (int argc, char* argv[])
{
  int n=20, fn;
  omp_set_dynamic(0);
#pragma omp parallel shared(n) num_threads(6)
  {
#pragma omp single
    fn = fib(n);
   }
  printf ("fib(%d) = %d\n", n, fn);
}
