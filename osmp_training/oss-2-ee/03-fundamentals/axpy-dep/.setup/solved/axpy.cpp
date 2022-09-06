#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <sys/time.h>

#pragma oss task in(x[0;N]) inout(y[0;N])
void axpy_task(double *x, double *y, double alpha, long N) {
	for (long i=0; i < N; ++i) {
		y[i] += alpha * x[i];
	}
}

void axpy(double *x, double *y, double alpha, long N, long TS) {
	for (long i=0; i < N; i+=TS) {
		axpy_task(x+i, y+i, alpha, std::min(TS, N-i));
	}
}

void multisaxpy(double *x, double *y, double alpha, long N, long TS, long its){
	for (long iteration=0; iteration < its; iteration++) {
		axpy(x, y, 1.0, N, TS);
	}
	#pragma oss taskwait
}

static void initialize(double *data, double value, long N, long TS) {
	for (long i=0; i < N; i++) {
		data[i] = value;
	}
}

int main(int argc, char **argv) {
	if (argc != 4) {
		std::cerr << "[USAGE] " << argv[0] << " elements chunksize iterations" << std::endl;
		return 1;
	}

	const long n = atoi(argv[1]);
	const long ts = atoi(argv[2]);
	const long its = atoi(argv[3]);

	double * const x = new double[n];
	double * const y = new double[n];
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

	std::cout << "size: " << n << ", bs: " << ts << ", iterations: " << its << ", time: " << duration << ", performance: " << performance << std::endl;

	return 0;
}
