#include <omp.h>
#include <stdio.h>
#include <float.h>
int main (int argc, const char * argv[]) {
        int i, n, chunk;
        float a[100], b[100], prod, minval;
/* Some initializations */
        n = 100;
        chunk = 10;
        for (i=0; i < n; i++) {
                a[i] = i * 1.0;
                b[i] = i * 2.0;
        }
        minval = FLT_MAX;
#pragma omp parallel for default(shared) private(i,prod) schedule(static,chunk)
        for (i=0; i < n; i++) {
                prod = a[i]*b[i];
#pragma omp critical
                minval = (prod < minval ? prod : minval);
        }
        printf("Minimum value = %f\n",minval);
}

