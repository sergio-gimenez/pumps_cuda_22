
#pragma oss task in([N]x) inout([N]y) device(openacc)
void axpy_openacc(double *x, double *y, double alpha, long N);

