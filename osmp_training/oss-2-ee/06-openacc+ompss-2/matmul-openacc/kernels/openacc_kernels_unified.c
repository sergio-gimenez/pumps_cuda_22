#include "openacc_matmul.h"


/* Basic, non-tiled matrix multiplication routine, run on the device
 * Assumes 3 2-D matrices of dimension MAT_DIM x MAT_DIM
 * Calculates C = A x B
 */
void mat_mul(double *A, double *B, double *C)
{
#pragma acc parallel loop collapse(2)
	for (int i = 0; i < MAT_DIM; i++)
		for (int j = 0; j < MAT_DIM; j++)
			for (int k = 0; k < MAT_DIM; k++)
				C[i * MAT_DIM + j] += A[i * MAT_DIM + k] * B[k * MAT_DIM + j];
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
#pragma acc parallel loop collapse(2)
	for  (int i = 0; i < tile_dim; i++)
		for (int j = 0; j < tile_dim; j++) {
			double sum = 0;
			for (int k = 0; k < tile_dim; k++)
				sum += A[i * MAT_DIM + k] * B[j * MAT_DIM + k];
			C[i * MAT_DIM + j] += sum;
		}
}

/* Routines initializing the matrices with dummy data */

void init_A(double *A)
{
#pragma acc parallel loop
	for (int i = 0; i < MAT_SIZE; i++) {
		A[i] = ((double)i/10.f + 3.f) * 0.000001f;
	}
}

void init_B(double *B)
{
#pragma acc parallel loop
	for (int i = 0; i < MAT_SIZE; i++) {
		B[i] = ((double)i/7.f - 2.f) * 0.000001f;
	}
}


