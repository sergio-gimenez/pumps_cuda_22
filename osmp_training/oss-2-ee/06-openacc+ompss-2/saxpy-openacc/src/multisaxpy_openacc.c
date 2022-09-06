#include "kernel.h"
#define ACC_DEFINE_CUDA_PTR(x) uint64_t acc_invisible_##x = (uint64_t)x;
#define ACC_ACCESS_CUDA_PTR(x) ((double*) acc_invisible_##x)




#ifndef OSS2EE_NANOS6_DIRECTORY


void axpy_openacc(double *x, double *y, double alpha, long N)
{
#pragma acc parallel loop independent
	for (long i = 0; i < N; i++)
		y[i] *= alpha * x[i];
}


#else


void axpy_openacc(double *x, double *y, double alpha, long N)
{
	ACC_DEFINE_CUDA_PTR(x);
	ACC_DEFINE_CUDA_PTR(y);
#pragma acc parallel loop independent
	for (long i = 0; i < N; i++)
		ACC_ACCESS_CUDA_PTR(y)[i] *= alpha * ACC_ACCESS_CUDA_PTR(x)[i];
}


#endif



void axpy(double *x, double *y, double alpha, long N, long TS)
{
	for (long i = 0; i < N; i += TS) {
		axpy_openacc(&x[i], &y[i], alpha, TS);
	}
}

void multisaxpy(double *x, double *y, double alpha, long N, long TS, long its)
{
	for (long iteration = 0; iteration < its; iteration++) {
		axpy(x, y, 1.0, N, TS);
	}
	#pragma oss taskwait
}

