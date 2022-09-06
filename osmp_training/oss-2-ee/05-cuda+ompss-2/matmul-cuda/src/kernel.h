
// Thread block size
#define BLOCK_SIZE 32

#ifdef DP
#define REAL double
#else
#define REAL float
#endif

#ifdef __cplusplus
extern "C"
{
#endif

//Kernel declaration as a task should be here
//Remember, we want to multiply two matrices, (A*B=C) where all of them have size NB*NB

#pragma oss task inout(([NB][NB]A)[0;NB][0;NB], ([NB][NB]B)[0;NB][0;NB]) inout(([NB][NB]C)[0;NB][0;NB]) device(cuda) ndrange(2, NB, NB, 32, 32)
__global__ void Muld(REAL* A, REAL* B, int wA, int wB, REAL* C, int NB);


#ifdef __cplusplus
}
#endif
