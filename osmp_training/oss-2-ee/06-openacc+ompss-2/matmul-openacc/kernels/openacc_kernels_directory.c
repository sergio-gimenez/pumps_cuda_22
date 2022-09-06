#include "openacc_matmul.h"


#define ACC_DEFINE_CUDA_PTR(x) uint64_t acc_invisible_##x = (uint64_t)x;
#define ACC_ACCESS_CUDA_PTR(x) ((double*) acc_invisible_##x)

/* Basic, non-tiled matrix multiplication routine, run on the device
 * Assumes 3 2-D matrices of dimension MAT_DIM x MAT_DIM
 * Calculates C = A x B
 */
void mat_mul(double *A, double *B, double *C)
{
ACC_DEFINE_CUDA_PTR(A);
ACC_DEFINE_CUDA_PTR(B);
ACC_DEFINE_CUDA_PTR(C);
#pragma acc parallel loop collapse(2)
	for (int i = 0; i < MAT_DIM; i++)
		for (int j = 0; j < MAT_DIM; j++)
			for (int k = 0; k < MAT_DIM; k++)
				ACC_ACCESS_CUDA_PTR(C)[i * MAT_DIM + j] += ACC_ACCESS_CUDA_PTR(A)[i * MAT_DIM + k] * ACC_ACCESS_CUDA_PTR(B)[k * MAT_DIM + j];
}

/* The 2-d matrix multiplication device routine, adapted to work in a tiled approach
 * Assumes A, B and C are pointers to the correct location of the first element of a tile,
 * B is transposed, and that all matrices are of total dimension MAT_DIM x MAT_DIM
 *
 * For example: if we have 16x16 matrices, they will consist of 256 lineraly allocated elements.
 * If we divide this in 4 tiles we will get 4 8x8 submatrices:
 *
 * 		| 0 | 1 |
 * 		| 2 | 3 |
 *
 * Because of the *linear* allocation of each matrix, however, tile 0 will contain elements 0-7, 16-23, 32-39 ... 112-119.
 * Similarly tile 1 will contain 8-15, 24-31 and so on. Hence, when we are calulating tile 1, the routine expects
 * to be given the pointer to element 8 --> &A[8]
 *
 */

void mmul_bt_kernel(double *A, double *B, double *C, unsigned int tile_dim)
{
	//not prepared to work with the directory
}


/* Routines initializing the matrices with dummy data */
void init_A(double *A)
{
    ACC_DEFINE_CUDA_PTR(A);

#pragma acc parallel loop
	for (int i = 0; i < MAT_SIZE; i++) {
		ACC_ACCESS_CUDA_PTR(A)[i] = ((double)i/10.f + 3.f) * 0.000001f;
	}
}

void init_B(double *B)
{
 ACC_DEFINE_CUDA_PTR(B);
#pragma acc parallel loop
	for (int i = 0; i < MAT_SIZE; i++) {
		ACC_ACCESS_CUDA_PTR(B)[i] = ((double)i/7.f - 2.f) * 0.000001f;
	}
}


