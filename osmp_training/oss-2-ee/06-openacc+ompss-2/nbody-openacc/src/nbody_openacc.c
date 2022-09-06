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
		calculate_force_func_openacc(bs,time_interval,number_of_particles,this_particle_array, &output_array[i], i, i+bs-1);
	}
}

//#pragma acc routine
void inline  calculate_force (
	Particle* this_particle1,
	Particle* this_particle2,
	float* force_x,
	float* force_y,
	float* force_z )
{
	float difference_x, difference_y, difference_z;
	float distance_squared, distance;
	float force_magnitude;

	difference_x = this_particle2->position_x - this_particle1->position_x;
	difference_y = this_particle2->position_y - this_particle1->position_y;
	difference_z = this_particle2->position_z - this_particle1->position_z;

	distance_squared =
		difference_x * difference_x +
		difference_y * difference_y +
		difference_z * difference_z;

	distance = sqrtf(distance_squared);

	force_magnitude =
		gravitational_constant *
		(this_particle1->mass) * (this_particle2->mass) / distance_squared;

	*force_x = (force_magnitude / distance) * difference_x;
	*force_y = (force_magnitude / distance) * difference_y;
	*force_z = (force_magnitude / distance) * difference_z;
}


void calculate_force_func_openacc (
	int size,
	float time_interval,
	int number_of_particles,
	Particle* d_particles,
	Particle *output,
	int first_local,
	int last_local )
{
#pragma acc parallel loop independent
	for (int p = first_local; p < last_local; p++) {

		Particle* this_particle = output + p - first_local;

		float force_x = 0.0f, force_y = 0.0f, force_z = 0.0f;
		float total_force_x = 0.0f, total_force_y = 0.0f, total_force_z = 0.0f;

		for (int i = 0; i < number_of_particles; i++) {
			if (i != p) {
				calculate_force(d_particles + p, d_particles + i, &force_x, &force_y, &force_z);

				total_force_x += force_x;
				total_force_y += force_y;
				total_force_z += force_z;
			}
		}

		float velocity_change_x, velocity_change_y, velocity_change_z;
		float position_change_x, position_change_y, position_change_z;

		this_particle->mass = d_particles[p].mass;

		velocity_change_x =
			total_force_x * (time_interval / this_particle->mass);
		velocity_change_y =
			total_force_y * (time_interval / this_particle->mass);
		velocity_change_z =
			total_force_z * (time_interval / this_particle->mass);

		position_change_x =
			d_particles[p].velocity_x + velocity_change_x * (0.5 * time_interval);
		position_change_y =
			d_particles[p].velocity_y + velocity_change_y * (0.5 * time_interval);
		position_change_z =
			d_particles[p].velocity_z + velocity_change_z * (0.5 * time_interval);

		this_particle->velocity_x = d_particles[p].velocity_x + velocity_change_x;
		this_particle->velocity_y = d_particles[p].velocity_y + velocity_change_y;
		this_particle->velocity_z = d_particles[p].velocity_z + velocity_change_z;

		this_particle->position_x = d_particles[p].position_x + position_change_x;
		this_particle->position_y = d_particles[p].position_y + position_change_y;
		this_particle->position_z = d_particles[p].position_z + position_change_z;
	}
}
