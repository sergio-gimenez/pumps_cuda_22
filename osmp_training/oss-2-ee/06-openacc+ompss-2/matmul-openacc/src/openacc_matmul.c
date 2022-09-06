#include <stdlib.h>
#include "openacc_matmul.h"

#ifdef DEBUG
#include <stdio.h>

/* A tile printing routine provided for convenience */
void p_tile(double *M, unsigned int tile_dim)
{
	printf("\n******************\n");
	for (int i = 0; i < tile_dim; i++) {
		for (int j = 0; j < tile_dim; j++)
			printf("%.1f\t", M[i* MAT_DIM + j]);
		printf("\n");
	}
	printf("******************\n\n");
}
#endif /* DEBUG */


/* Transpose matrix, there are certainly more memory efficient ways but let's just create another copy */
void mat_transpose(double *M, double *tr_M)
{
	for (int i = 0; i < MAT_DIM; i++)
		for (int j = 0; j < MAT_DIM; j++)
			tr_M[i * MAT_DIM + j] = M[j * MAT_DIM + i];
}

/* ***************** The top-level tiled-multiplication call **********************
 * A, B:	Input matrices
 * C:		Output matrix
 * div:		divider, e.g. will use 4*4 (=16) tiles
 *
 * This routine will spawn an OmpSs-2 task for each tile.
 * Hence this becomes already multithreaded at top-level before any GPU parallelism.
 */
void mat_mul_tiled(double *A, double *B, double *C,
					unsigned short div)
{
	unsigned int tile_dim = MAT_DIM / div;	/* assume division is exact, obviously */

	/* For each tile of result matrix */
	for (int i = 0; i < div; i++) {
		for (int j = 0; j < div; j++) {

			compute_result_tile(A, B, C, div, tile_dim, i, j);
		}
	}
}
/* ******************* Compute a tile of the result matrix *******************
 * A, B:	 Input matrices
 * C:		 Output matrix
 * div:		 number of divisions per dimension (of the initial matrix)
 * tile_dim: tile dimension ==> each tile contains tile_dim*tile_dim elements
 * x,y:		 the relative coordinates of the tile. Used to calculate offset from base pointer
 *
 * 	E.g if we have 16 tiles:
 * 		div will be 4
 * 		x,y will be (0,0) (0,1)..(3,2) (3,3),
 * 		indicating 'left-most upper-most' tile for (0,0) and so on
 *
 * This calls the bottom-level multiplication kernel for each combination of A-B tiles required for
 * 1 tile of C
 */
void compute_result_tile(double *A, double *B,
						 double *C, unsigned short div,
						 unsigned int tile_dim,	int x, int y)
{
	/* This is not so easy to read but does the same as having double pointer (M)atrices M[][].
	 * Each 'temp_M' actually gets the starting position of the tile
	 * The access pattern for each M = tiled_M[x][y] should be:
	 * 	for i in tile _dim
	 * 		for j in tile dim
	 * 			M[i * MAT_DIM + j]
	 */

	/* Do the 'div' times partial results */

	double *temp_C = &C[x * tile_dim * MAT_DIM + y * tile_dim];
	for (int d = 0; d < div; d++) {
		double *temp_A = &A[x * tile_dim * MAT_DIM + d * tile_dim];
		double *temp_B = &B[y * tile_dim * MAT_DIM + d * tile_dim];

		mmul_bt_kernel(temp_A, temp_B, temp_C, tile_dim);
	}

}

