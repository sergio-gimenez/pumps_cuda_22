#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

long N;
long CHUNK_SIZE;

void initialize(long length, long chunk_size, double data_A[length], double data_B[length])
{
    long n_chunks;
    long actual_size;

    n_chunks = length/chunk_size;
    n_chunks = (n_chunks*chunk_size < length)? n_chunks + 1 : n_chunks;

    for (long i = 0; i < length; i += chunk_size) {

        actual_size = (length - i >= chunk_size)? chunk_size : (length - i);

        #pragma oss task out(data_A[i;actual_size]) out(data_B[i;actual_size])
        {
            for (long ii = 0; ii < actual_size; ++ii) {
                data_A[i + ii] = i + ii + 1;
                data_B[i + ii] = 1;
            }
        }
    }
}

double dot_product(long N, long CHUNK_SIZE, double A[N], double B[N])
{
    long N_CHUNKS;
    long actual_size;
    double acc;

    N_CHUNKS = N/CHUNK_SIZE;
    N_CHUNKS = (N_CHUNKS*CHUNK_SIZE < N)? N_CHUNKS + 1 : N_CHUNKS;

    acc = 0.0;
    for (long i = 0; i < N; i += CHUNK_SIZE) {

        actual_size = (N - i >= CHUNK_SIZE)? CHUNK_SIZE : (N - i);

        #pragma oss task in(A[i;actual_size], B[i;actual_size]) reduction(+: acc)
        {
            for (long ii = 0; ii < actual_size; ii++)
                acc += A[i + ii]*B[i + ii];
        }
    }

    #pragma oss taskwait
    return acc;
}


int main(int argc, char *argv[]) {
    struct timeval start;
    struct timeval stop;
    unsigned long elapsed;
    double result;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <vector size in K> <chunk size in K>\n", argv[0]);
        return 1;
    }

    N = atol(argv[1])*1024L;
    CHUNK_SIZE = atol(argv[2])*1024L;

    double *A = (double*)malloc(N*sizeof(double));
    double *B = (double*)malloc(N*sizeof(double));

    if (A == NULL || B == NULL)
    {
        printf("Error: Can't allocate %ld Bytes\nExiting.\n", N*sizeof(double));
        exit(1);
    }

    gettimeofday(&start, NULL);

    initialize(N, CHUNK_SIZE, A, B);

    result = dot_product(N, CHUNK_SIZE, A, B);

    gettimeofday(&stop, NULL);
    elapsed = 1000000*(stop.tv_sec - start.tv_sec);
    elapsed += stop.tv_usec - start.tv_usec;

    printf("size (K), %lu, chunk (K), %lu, time (ms), %lu\n", N/1024, CHUNK_SIZE/1024, elapsed/1000);
}
