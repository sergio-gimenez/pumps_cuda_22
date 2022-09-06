#include <iostream>


extern void multisaxpy(double *x, double *y, double alpha, long N, long TS, long its);

extern void *allocate(size_t size);
extern void *deallocate(void *data);

static void initialize(double *data, double value, long N, long TS) {
	for (long i=0; i < N; i++) {
		data[i] = value;
	}
}

int main(int argc, char* argv[])
{
	if (argc != 4) {
		std::cout << "Usage: ./saxpy [Size] [Blocksize] [Iterations] \n" \
			<< "\tSize\t\t: Integer - Number of elements of the vector \n" \
			<<"\tBlocksize\t: Integer - Number of element to calculate in each task. \n" \
			<<"\tIterations\t: Integer - Number of iterations to perform. \n" \
			<< std::endl;
		return 0;
	}
	
	const long n = atoi(argv[1]);
	const long ts = atoi(argv[2]);
	const long its = atoi(argv[3]);
	
	double *x = (double *) allocate(n*sizeof(double));
	double *y = (double *) allocate(n*sizeof(double));
	
	initialize(x, 1.0, n, ts);
	initialize(y, 0.0, n, ts);
	
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);	

	multisaxpy(x, y, 1.0, n, ts, its);
		
	clock_gettime(CLOCK_MONOTONIC, &end);

	double duration = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
	duration /= 1000000;

	double performance = n;
	performance *= its;
	performance = performance / duration;
	performance /= 1000000000;

	std::cout << n << ", " << ts << ", " << its << ", " << duration << ", " << performance << std::endl;

	deallocate(x);
	deallocate(y);
	
	return 0;
}
