#include "kernel.h"

void axpy(double *x, double *y, double alpha, long N, long TS)
{
	for (long i = 0; i < N; i += TS) {
		axpy_cuda(&x[i], &y[i], alpha, TS);
	}
}

void multisaxpy(double *x, double *y, double alpha, long N, long TS, long its)
{
	for (long iteration = 0; iteration < its; iteration++) {
		axpy(x, y, 1.0, N, TS);
	}
	#pragma oss taskwait
}

