#include <stdio.h>
#include <stdlib.h>
#define WIDTH 500

void matmul(float* M, float* N, float* P, int Width) 
{
    int i, j, k;
    for (i = 0; i < Width; ++i)
        for (j = 0; j < Width; ++j) {
            float sum = 0;
            for (k = 0; k < Width; ++k) {
                float a = M[i * Width + k];
                float b = N[k * Width + j];
                sum += a * b;
            }
            P[i * Width + j] = sum;
        }
}

void matmul_omp(float* A, float* B, float* C, int Width)
{
    int i, j, k;
    float sum;
#pragma omp parallel for collapse(2) default(shared) private(k,sum) 
    for (i = 0; i < Width; ++i)
        for (j = 0; j < Width; ++j) {
            sum = 0.0;
            for (k = 0; k < Width; ++k) {
                sum += A[i * Width + k]*B[k * Width + j];
            }
            C[i * Width + j] = sum;
        }
}

int main()
{
    float *M, *N, *P, *A, *B, *C;
    int i, n = WIDTH, size=sizeof(float)*n*n;
    P = (float *)malloc(size);
    M = (float *)malloc(size);
    N = (float *)malloc(size);
    A = (float *)malloc(size);
    B = (float *)malloc(size);
    C = (float *)malloc(size);
    for(i=0;i<n*n;i++){
	*(M+i)=(float)i; *(N+i)=(float)i;
	*(A+i)=(float)i; *(B+i)=(float)i;
    }
    matmul(M,N,P,n);
    matmul_omp(A,B,C,n);
    int ok=1;
    for(i=0;i<n*n;i++) ok &= (*(C+i) == *(P+i));
    if (ok) printf("Everything worked\n");
    else printf("Something went wrong\n");
}
