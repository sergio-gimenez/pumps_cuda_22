#include "kernel.h"

void Particle_array_calculate_forces(
	Particle* this_particle_array, 
	Particle *output_array, 
	int number_of_particles, 
	float time_interval, 
	int timestep) 
{
	const int bs = number_of_particles/32; // TODO Configurable block number
	
	for (int i = 0; i < number_of_particles; i += bs) { 
		calculate_force_func_cuda(bs,time_interval,number_of_particles,this_particle_array, &output_array[i], i, i+bs-1);	
	}
}

