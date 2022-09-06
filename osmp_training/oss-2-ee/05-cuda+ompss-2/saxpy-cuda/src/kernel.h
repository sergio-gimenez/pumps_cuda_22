#ifdef __cplusplus
extern "C"
{
#endif

#pragma oss task in([N]x) inout([N]y) device(cuda) ndrange(1, N, 128)
__global__ void axpy_cuda(double *x, double *y, double alpha, long N);

#ifdef __cplusplus
}
#endif
