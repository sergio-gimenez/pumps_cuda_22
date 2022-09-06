/* nbody.c */

#include "nbody.h"

extern void Particle_array_calculate_forces(
	Particle* this_particle_array, 
	Particle *output_array, 
	int number_of_particles, 
	float time_interval , 
	int timestep);


double wall_time ( )
{
	struct timespec ts;
	double res;
	
	clock_gettime(CLOCK_MONOTONIC,&ts);
	
	res = (double) (ts.tv_sec)  + (double) ts.tv_nsec * 1.0e-9;
	
	return res;
}

int main (int argc, char** argv)
{ 
	Particle* particle_array = (Particle*) NULL;
	Particle* particle_array2 = (Particle*) NULL;
	int timestep;
	int i;
	
	FILE *input_data = fopen(argv[1], "r");
	Particle_input_arguments(input_data);
	
	particle_array = Particle_array_construct(number_of_particles);
	particle_array2 = Particle_array_construct(number_of_particles);

	Particle_array_initialize(particle_array, number_of_particles);

	FILE * fileptr = fopen("nbody_out.xyz", "w");
	Particle_array_output_xyz(fileptr, particle_array, number_of_particles);
	
	if (number_of_particles <= 1) {
		printf("Returning\n");		
		return 0;
	}
	
	Particle_array_initialize(particle_array, number_of_particles);
	    
	double start = wall_time();
	        
	for (timestep = 1; timestep <= number_of_timesteps; timestep++) {
		if ((timestep % timesteps_between_outputs) == 0 ) fprintf(stderr, "Starting timestep #%d.\n", timestep);
	
		Particle_array_calculate_forces(particle_array, particle_array2, number_of_particles, time_interval , timestep);
		
		Particle * tmp = particle_array;
		particle_array = particle_array2;
		particle_array2 = tmp;
		
	}
	
	#pragma oss taskwait
	double end = wall_time ();
	
	printf("Time in seconds: %g s.\n", end - start);
	printf("Particles per second: %g \n", (number_of_particles*number_of_timesteps)/(end-start));
	
	if ((number_of_timesteps % timesteps_between_outputs) != 0) {
		Particle_array_output_xyz(fileptr, particle_array, number_of_particles);
	}
	
	particle_array = Particle_array_destruct(particle_array, number_of_particles);
	
	if (fclose(fileptr) != 0) {
		fprintf(stderr, "ERROR: can't close the output file.\n");
		exit(program_failure_code);
	}
	
	return program_success_code;
}
