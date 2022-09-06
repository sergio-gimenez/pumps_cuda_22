#include "kernel.h"

#include <stdio.h>

__global__ void axpy_cuda(double *x, double *y, double alpha, long N)
{
   long int i = blockIdx.x * blockDim.x + threadIdx.x;
   if(i < N) y[i] *= alpha * x[i];
}
