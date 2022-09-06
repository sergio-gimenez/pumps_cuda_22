#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <kernel.h>
#include <malloc.h>
#include "driver.h"
#include <cuda_runtime_api.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#define US_DIR

#ifdef DP
#define REAL double
#else
#define REAL float
#endif

#define ALIGNMENT 4096

const int NB = BSIZE;

void prtspeed( int m, int l, int n, int nb, double time, int ok, unsigned long nops );
double   cclock( void );
int      check( int nrep, int m, int l, int n, int mDIM, int nDIM, REAL **c );
void     gendat(int, int, int, int, int, int, REAL **, REAL **, REAL **);
void     prthead( void );

void matmul( int m, int l, int n, int mDIM, int lDIM, int nDIM, REAL **tileA, REAL **tileB, REAL **tileC )
{
	int i, j, k,o, ok = 0;
		double eps, tvalue = (double)l;
	eps = 2.0*l*l*DBL_EPSILON;

	int perfectM = m / BSIZE;
	int perfectN = n / BSIZE;

	int leftOutM = m % BSIZE;
	int leftOutN = n % BSIZE;
	for(i = 0;i < mDIM; i++){
		for (j = 0; j < nDIM; j++){
			for (k = 0; k < lDIM; k++){
				Muld(tileA[i*lDIM+k], tileB[k*nDIM+j],NB,NB, tileC[i*nDIM+j],NB);
			}
		}
	}
}


int calcdim(int x)
{
   int dimval;

   if(x%BSIZE != 0) dimval = x/BSIZE + 1;
   else dimval = x/BSIZE;

   return dimval;
}

void *allocate(size_t size)
{
#ifdef OSS2EE_NANOS6_DIRECTORY
   // Allocate regular memory
   return malloc(size);
#else
   // Allocate managed memory
   void *data;
   cudaMallocManaged((void **) &data, size, cudaMemAttachGlobal);
   return data;
#endif
}

void deallocate(void *data)
{
#ifdef OSS2EE_NANOS6_DIRECTORY
   free(data);
#else
   cudaFree(data);
#endif
}

int main()
{
	int      lda, m, l, n;
	int      mDIM, lDIM, nDIM;
	int      ok, nrep;
	unsigned long nops;
	int      i,k,j,o;
	REAL   **a, **b, **c;
	double   time;
	FILE     *inl;

	inl = fopen( "test.in", "r" );
	if (inl == 0) {
		printf("No input file 'test.in' found.\n");
		exit(1);
	}

	printf("Selected Block Size: %d\n", BSIZE);

	while( ( fscanf( inl, "%d%d%d%d\n", &m, &l, &n, &nrep ) != EOF ) ) {
		lda = l + 1;

		mDIM = calcdim(m);
		lDIM = calcdim(l);
		nDIM = calcdim(n);

		a = (REAL **) malloc( mDIM*lDIM*sizeof( REAL *) );
		b = (REAL **) malloc( lDIM*nDIM*sizeof( REAL *) );
		c = (REAL **) malloc( mDIM*nDIM*sizeof( REAL *) );

		for(i=0;i<mDIM*lDIM;i++) a[i] = allocate( BSIZE*BSIZE*sizeof(REAL) );
		for(i=0;i<lDIM*nDIM;i++) b[i] = allocate( BSIZE*BSIZE*sizeof(REAL) );
		for(i=0;i<mDIM*nDIM;i++) c[i] = allocate( BSIZE*BSIZE*sizeof(REAL) );

		gendat( mDIM, lDIM, nDIM, m, l, n, a, b, c );

		time = cclock();
        for(int i = 0; i < nrep;++i)
			matmul( m, l, n, mDIM, lDIM, nDIM, a, b, c );
		#pragma oss taskwait

		time = cclock() - time;
		ok   = check( nrep, m, l, n, mDIM, nDIM, c);
		time = time/nrep;
		nops  = (unsigned long) 2*m*l*n;
		prtspeed( m, l, n, BSIZE, time, ok, nops );

		for(i=0;i<mDIM*lDIM;i++) deallocate( a[i] );
		for(i=0;i<lDIM*nDIM;i++) deallocate( b[i] );
		for(i=0;i<mDIM*nDIM;i++) deallocate( c[i] );

		free( a ); free( b ); free( c );
	}
}
