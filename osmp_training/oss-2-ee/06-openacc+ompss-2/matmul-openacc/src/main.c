#include <stdio.h>
#include <stdlib.h>
#include "openacc_matmul.h"

double  cclock(void);

int main(int argc, char *argv[])
{
	int mode = 0;	/* Multiplication mode: 0 -> non-tiled, 1 -> tiled. Default: 0 */
	int div  = 8;	/* Number of dimension divisions for tiled mode. Default: 8*8 = 64 tiles */
	if (argc > 1) {
		mode = (atoi(argv[1]) == 0 || atoi(argv[1]) == 1) ? atoi(argv[1]) : 0;
		if (argc > 2)
			div = atoi(argv[2]);
	}

	double *A, *B, *C;
	double *tr_B; 		/* B transposed, so that we can have the same access pattern
						 * for A and B during tiled multiplication */

	double time;
	A = (double *)calloc(MAT_SIZE , sizeof(double));
	B = (double *)calloc(MAT_SIZE , sizeof(double));
	C = (double *)calloc(MAT_SIZE , sizeof(double));
	if (mode)  /* Tiled approach */
		tr_B = (double *)calloc(MAT_SIZE , sizeof(double));

	/* Init matrices
	 *
	 * These routines initialize A and B with dummy values,
	 * just for the sake of computation.
	 */

	init_A(A);

	init_B(B);


	time = cclock();
	/* Proceed with matrix multiply*/
	if (mode) { 	/* Tiled approach */
	/* Transpose B */
#pragma oss task in([MAT_SIZE]B) out([MAT_SIZE]tr_B)
		mat_transpose(B, tr_B);
#pragma oss task in([MAT_SIZE]A, [MAT_SIZE]tr_B) out([MAT_SIZE]C)
		mat_mul_tiled(A, tr_B, C, div);
	}
	else /* non-tiled approach */
		mat_mul(A, B, C);

#pragma oss taskwait

	time = cclock() - time;
	printf("\nComputation time (in seconds): %.4lf\n\n", time);

	/* Print a randomly chosen element for result verification
	 *
	 * Correct expected value: 16071.95430185 for MAT_DIM 8192, MAT_SIZE 67108864 */
	if (mode)
		printf("Tiled: Showing element 40883230 of the result: %.8f\n", C[4088320]);
	else
		printf("Non-tiled: Showing element 40883230 of the result: %.8f\n", C[4088320]);

	free(A);
	free(B);
	free(C);

	if (mode)
		free(tr_B);

	return 0;
}
