#include "nbody.h"

#include <mpi.h>

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <ieee754.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


void nbody_particle_init(const nbody_conf_t *conf, particles_block_t *part)
{
	for (int i = 0; i < BLOCK_SIZE; i++){
		part->position_x[i] = conf->domain_size_x * ((float)random() / ((float)RAND_MAX + 1.0));
		part->position_y[i] = conf->domain_size_y * ((float)random() / ((float)RAND_MAX + 1.0));
		part->position_z[i] = conf->domain_size_z * ((float)random() / ((float)RAND_MAX + 1.0));
		part->mass[i] = conf->mass_maximum * ((float)random() / ((float)RAND_MAX + 1.0));
		part->weight[i] = gravitational_constant * part->mass[i];
	}
}

int nbody_compare_particles(const particles_block_t *local, const particles_block_t *reference, int num_blocks)
{
	double error = 0.0;
	int count = 0;
	for (int i = 0; i < num_blocks; i++) {
		for (int e = 0; e < BLOCK_SIZE; e++) {
			if ((local[i].position_x[e] != reference[i].position_x[e]) ||
			    (local[i].position_y[e] != reference[i].position_y[e]) ||
			    (local[i].position_z[e] != reference[i].position_z[e])) {
					error += fabs(((local[i].position_x[e] - reference[i].position_x[e])*100.0) / reference[i].position_x[e]) +
					         fabs(((local[i].position_y[e] - reference[i].position_y[e])*100.0) / reference[i].position_y[e]) +
					         fabs(((local[i].position_z[e] - reference[i].position_z[e])*100.0) / reference[i].position_z[e]);
					count++;
			}
		}
	}
	
	double relative_error = (count != 0) ? error / (3.0 * count) : 0.0;
	if ((count * 100.0) / (num_blocks * BLOCK_SIZE) > 0.6 || relative_error > TOLERATED_ERROR) {
		return 0;
	}
	return 1;
}

void nbody_generate_particles(const nbody_conf_t *conf, const nbody_file_t *file)
{
	int rank_size;
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
	
	char fname[1024];
	sprintf(fname, "%s.in", file->name);
	
	if (!access(fname, F_OK)) {
		return;
	}
	
	struct stat st = {0};
	if (stat("data", &st) == -1) {
		mkdir("data", 0755);
	}
	
	const int fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IRGRP|S_IROTH);
	assert(fd >= 0);
	
	const int total_size = file->total_size;
	assert(total_size % PAGE_SIZE == 0);
	
	int err = ftruncate(fd, total_size);
	assert(!err);
	
	particles_block_t * const particles = mmap(NULL, total_size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	
	for(int i = 0; i < conf->num_blocks * rank_size; i++) {
		nbody_particle_init(conf, particles+i);
	}
	
	err = munmap(particles, total_size);
	assert(!err);
	
	err = close(fd);
	assert(!err);
}

void nbody_check(const nbody_t *nbody)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	char fname[1024];
	sprintf(fname, "%s.ref", nbody->file.name);
	if (access(fname, F_OK) != 0) {
		if (!rank) fprintf(stderr, "Warning: %s file does not exist. Skipping the check...\n", fname);
		return;
	}
	
	const int fd = open(fname, O_RDONLY, 0);
	assert(fd >= 0);
	
	particles_block_t *reference = mmap(NULL, nbody->file.size, PROT_READ, MAP_SHARED, fd, nbody->file.offset);
	assert(reference != MAP_FAILED);
	
	int correct, correct_chunk;
	correct_chunk = nbody_compare_particles(nbody->local, reference, nbody->num_blocks);
	
	MPI_Reduce(&correct_chunk, &correct, 1, MPI_INT, MPI_LAND, 0, MPI_COMM_WORLD);
	
	if (!rank) {
		if (correct) {
			printf("Result validation: OK\n");
		} else {
			printf("Result validation: ERROR\n");
		}
	}
	
	int err = munmap(reference, nbody->file.size);
	assert(!err);
	
	err = close(fd);
	assert(!err);
}

nbody_file_t nbody_setup_file(const nbody_conf_t *conf)
{
	int rank, rank_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
	
	nbody_file_t file;
	file.size = conf->num_blocks * sizeof(particles_block_t);
	file.total_size = rank_size * file.size;
	file.offset = rank * file.size;
	
	sprintf(file.name, "%s-%s-%d-%d-%d", conf->name, TOSTRING(BIGO), rank_size * conf->num_blocks * BLOCK_SIZE, BLOCK_SIZE, conf->timesteps);
	return file;
}

particles_block_t * nbody_load_particles(const nbody_conf_t *conf, const nbody_file_t *file)
{
	char fname[1024];
	sprintf(fname, "%s.in", file->name);
	
	const int fd = open(fname, O_RDONLY, 0);
	assert(fd >= 0);
	
	void * const ptr = mmap(NULL, file->size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, file->offset);
	assert(ptr != MAP_FAILED);
	
	int err = close(fd);
	assert(!err);
	
	return ptr;
}

nbody_t nbody_setup(const nbody_conf_t *conf)
{
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	nbody_file_t file = nbody_setup_file(conf);
	
	if (!rank) nbody_generate_particles(conf, &file);
	MPI_Barrier(MPI_COMM_WORLD);
	
	nbody_t nbody;
	nbody.local = nbody_load_particles(conf, &file);
	assert(nbody.local != NULL);
	
	nbody.remote1 = nbody_alloc(conf->num_blocks * sizeof(particles_block_t));
	nbody.remote2 = nbody_alloc(conf->num_blocks * sizeof(particles_block_t));
	assert(nbody.remote1 != NULL);
	assert(nbody.remote2 != NULL);
	
	nbody.forces = nbody_alloc(conf->num_blocks * sizeof(forces_block_t));
	assert(nbody.forces != NULL);
	
	nbody.num_blocks = conf->num_blocks;
	nbody.timesteps = conf->timesteps;
	nbody.file = file;
	
	return nbody;
}

void nbody_save_particles(const nbody_t *nbody)
{
	int rank, rank_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);
	
	char fname[1024];
	sprintf(fname, "%s.out", nbody->file.name);
	const int mode = MPI_MODE_CREATE | MPI_MODE_WRONLY;
	
	MPI_File outfile;
	int err = MPI_File_open(MPI_COMM_WORLD, fname, mode, MPI_INFO_NULL, &outfile);
	assert(err == MPI_SUCCESS);
	
	MPI_File_set_view(outfile, nbody->file.offset, MPI_BYTE, MPI_BYTE, "native", MPI_INFO_NULL);
	assert(err == MPI_SUCCESS);
	
	MPI_File_write(outfile, nbody->local, nbody->file.size, MPI_BYTE, MPI_STATUS_IGNORE);
	assert(err == MPI_SUCCESS);
	
	MPI_File_close(&outfile);
	assert(err == MPI_SUCCESS);
	
	MPI_Barrier(MPI_COMM_WORLD);
}

void nbody_free(nbody_t *nbody)
{
	const int particles_size = nbody->num_blocks * sizeof(particles_block_t);
	const int forces_size = nbody->num_blocks * sizeof(forces_block_t);
	
	int err = munmap(nbody->local, particles_size);
	err |= munmap(nbody->remote1, particles_size);
	err |= munmap(nbody->remote2, particles_size);
	err |= munmap(nbody->forces, forces_size);
	assert(!err);
}

void * nbody_alloc(size_t size)
{
	void *addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	assert(addr != MAP_FAILED);
	return addr;
}

void nbody_print_usage(int argc, char **argv)
{
	fprintf(stderr, "Usage: %s <-p particles> <-t timesteps> [OPTION]...\n", argv[0]);
	fprintf(stderr, "Parameters:\n");
	fprintf(stderr, "  -p, --particles=PARTICLES\t\tuse PARTICLES as the total number of particles (default: 16384)\n");
	fprintf(stderr, "  -t, --timesteps=TIMESTEPS\t\tuse TIMESTEPS as the number of timesteps (default: 10)\n\n");
	fprintf(stderr, "Optional parameters:\n");
	fprintf(stderr, "  -c, --check\t\t\t\tcheck the correctness of the result (disabled by default)\n");
	fprintf(stderr, "  -C, --no-check\t\t\tdo not check the correctness of the result\n");
	fprintf(stderr, "  -o, --output\t\t\t\tsave the computed particles to the default output file (disabled by default)\n");
	fprintf(stderr, "  -O, --no-output\t\t\tdo not save the computed particles to the default output file\n");
	fprintf(stderr, "  -h, --help\t\t\t\tdisplay this help and exit\n\n");
}

nbody_conf_t nbody_get_conf(int argc, char **argv)
{
	nbody_conf_t conf;
	conf.domain_size_x = default_domain_size_x;
	conf.domain_size_y = default_domain_size_y;
	conf.domain_size_z = default_domain_size_z;
	conf.mass_maximum  = default_mass_maximum;
	conf.time_interval = default_time_interval;
	conf.seed          = default_seed;
	conf.name          = default_name;
	conf.num_particles = default_num_particles;
	conf.num_blocks    = conf.num_particles / BLOCK_SIZE;
	conf.timesteps     = default_timesteps;
	conf.save_result   = default_save_result;
	conf.check_result  = default_check_result;
	
	static struct option long_options[] = {
		{"particles",	required_argument,	0, 'p'},
		{"timesteps",	required_argument,	0, 't'},
		{"check",		no_argument,		0, 'c'},
		{"no-check",	no_argument,		0, 'C'},
		{"output",		no_argument,		0, 'o'},
		{"no-output",	no_argument,		0, 'O'},
		{"help",		no_argument,		0, 'h'},
		{0, 0, 0, 0}
	};
	
	int c;
	int index;
	while ((c = getopt_long(argc, argv, "hoOcCp:t:", long_options, &index)) != -1) {
		switch (c) {
			case 'h':
				nbody_print_usage(argc, argv);
				exit(0);
			case 'o':
				conf.save_result = 1;
				break;
			case 'O':
				conf.save_result = 0;
				break;
			case 'c':
				conf.check_result = 1;
				break;
			case 'C':
				conf.check_result = 0;
				break;
			case 'p':
				conf.num_particles = atoi(optarg);
				break;
			case 't':
				conf.timesteps = atoi(optarg);
				break;
			case '?':
				exit(1);
			default:
				abort();
		}
	}
	
	if (!conf.num_particles || !conf.timesteps) {
		int rank;
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		if (rank == 0) nbody_print_usage(argc, argv);
		exit(1);
	}
	
	return conf;
}

double nbody_compute_throughput(int num_particles, int timesteps, double elapsed_time)
{
	double interactions_per_timestep = 0;
#if defined(_BIGO_N2)
	interactions_per_timestep = (double)(num_particles)*(double)(num_particles);
#elif defined(_BIGO_NlogN)
	interactions_per_timestep = (double)(num_particles)*(double)(LOG2(num_particles));
#elif defined(_BIGO_N)
	interactions_per_timestep = (double)(num_particles);
#endif
	return (((interactions_per_timestep * (double)timesteps) / elapsed_time) / 1000000.0);
}

double get_time()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)(ts.tv_sec) + (double)ts.tv_nsec * 1.0e-9;
}
