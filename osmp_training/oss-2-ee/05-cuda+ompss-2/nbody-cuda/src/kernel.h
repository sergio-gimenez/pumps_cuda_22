#include"nbody.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma oss task in([size]d_particles) inout([size]output) device(cuda) ndrange(1, size, 128)
__global__ void calculate_force_func_cuda(
	int size, 
	float time_interval, 
	int number_of_particles, 
	Particle* d_particles, 
	Particle *output, 
	int first_local, 
	int last_local );

#ifdef __cplusplus
}
#endif
