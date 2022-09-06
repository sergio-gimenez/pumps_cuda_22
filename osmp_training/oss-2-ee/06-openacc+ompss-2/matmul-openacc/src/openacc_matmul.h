
//#define MAT_SIZE 67108864
//#define MAT_DIM	 8192
//#define MAT_SIZE 16777216
//#define MAT_DIM	 4096
//#define MAT_SIZE 16384
//define MAT_DIM	 128

/* MAT_SIZE = MAT_DIM ^2 */

/* enum instead of #define to work-around specific Mercurium compiler limitations in oss clauses */
enum {
	MAT_DIM = 8192,
	MAT_SIZE = 67108864
};

#pragma oss task device(openacc) out([MAT_SIZE]A)
void init_A(double *A);


#pragma oss task device(openacc) out([MAT_SIZE]B)
void init_B(double *B);

void mat_transpose(double *M, double *tr_M);

#pragma oss task device(openacc) in([MAT_SIZE]A, [MAT_SIZE]B) out([MAT_SIZE]C)
void mat_mul(double *A, double *B, double *C);

void mat_mul_tiled(double *A, double *B, double *C,	unsigned short div);

void compute_result_tile(double *A, double *B,
						 double *C, unsigned short div,
						 unsigned int tile_dim,	int x, int y);

void mmul_bt_kernel(double *A, double *B, double *C, unsigned int tile_dim);

