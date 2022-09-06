#include"nbody.h"

#pragma oss task in([size]d_particles) inout([size]output) device(openacc)
void calculate_force_func_openacc(
	int size,
	float time_interval,
	int number_of_particles,
	Particle* d_particles,
	Particle *output,
	int first_local,
	int last_local );

