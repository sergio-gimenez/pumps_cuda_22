#include "nbody.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mpi.h>
#include <nanos6/debug.h>
#include <TAMPI.h>

static void calculate_forces(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2, const int num_blocks);
static void update_particles(particles_block_t *particles, forces_block_t *forces, const int num_blocks, const float time_interval);
static void exchange_particles(const particles_block_t *sendbuf, particles_block_t *recvbuf, const int num_blocks, const int tagbase, const int rank, const int rank_size);
static void calculate_forces_block(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2);
static void update_particles_block(particles_block_t *particles, forces_block_t *forces, const float time_interval);
static void send_particles_block(const particles_block_t *sendbuf, int block_id, int dst);
static void recv_particles_block(particles_block_t *recvbuf, int block_id, int src);
static int get_tagbase(int iteration, int num_blocks);

int serial;

void nbody_solve(nbody_t *nbody, const int num_blocks, const int timesteps, const float time_interval)
{
	assert(nbody != NULL);
	assert(timesteps > 0);

	int rank, rank_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

	particles_block_t *local = nbody->local;
	particles_block_t *remote1 = nbody->remote1;
	particles_block_t *remote2 = nbody->remote2;
	forces_block_t *forces = nbody->forces;

	for (int t = 0; t < timesteps; t++) {
		particles_block_t *sendbuf = local;
		particles_block_t *recvbuf = remote1;

		for (int r = 0; r < rank_size; r++) {
			calculate_forces(forces, local, sendbuf, num_blocks);
			if (r < rank_size - 1) {
				// Since we leverage a double buffering technique, we
				// need to define different MPI tag domains for each
				// communication buffer. Otherwise, we would use the
				// same tags from concurrent communication tasks. In
				// fact, this is only needed when enabling TAMPI (no
				// serialization of communication tasks).
				int tagbase = get_tagbase(r, num_blocks);

				exchange_particles(sendbuf, recvbuf, num_blocks, tagbase, rank, rank_size);
			}

			particles_block_t *aux = recvbuf;
			recvbuf = (r != 0) ? sendbuf : remote2;
			sendbuf = aux;
		}

		update_particles(local, forces, num_blocks, time_interval);
	}

	#pragma oss taskwait
	MPI_Barrier(MPI_COMM_WORLD);
}

void calculate_forces_N2(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2, const int num_blocks)
{
	for (int i = 0; i < num_blocks; i++) {
		for (int j = 0; j < num_blocks; j++) {
			calculate_forces_block(forces+i, block1+i, block2+j);
		}
	}
}

void calculate_forces_NlogN(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2, const int num_blocks)
{
	for (int i = 0; i < num_blocks; i++) {
		for (int j = 0; j < LOG2(num_blocks); j++) {
			calculate_forces_block(forces+i, block1+i, block2+j);
		}
	}
}

void calculate_forces_N(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2, const int num_blocks)
{
	for (int i = 0; i < num_blocks - 1; i++) {
		calculate_forces_block(forces+i, block1+i, block2+i+1);
	}
}

void update_particles(particles_block_t *particles, forces_block_t *forces, const int num_blocks, const float time_interval)
{
	for (int i = 0; i < num_blocks; i++) {
		update_particles_block(particles+i, forces+i, time_interval);
	}
}

void exchange_particles(const particles_block_t *sendbuf, particles_block_t *recvbuf, const int num_blocks, const int tagbase, const int rank, const int rank_size)
{
	int src = MOD(rank - 1, rank_size);
	int dst = MOD(rank + 1, rank_size);
	int size = sizeof(particles_block_t);

	if (rank % 2) {
		for (int i = 0; i < num_blocks; i++) {
			send_particles_block(sendbuf+i, tagbase+i, dst);
			recv_particles_block(recvbuf+i, tagbase+i, src);
		}
	} else {
		for (int i = 0; i < num_blocks; i++) {
			recv_particles_block(recvbuf+i, tagbase+i, src);
			send_particles_block(sendbuf+i, tagbase+i, dst);
		}
	}
}

#pragma oss task label("calculate_forces_block") in(*block1, *block2) inout(*forces)
void calculate_forces_block(forces_block_t *forces, const particles_block_t *block1, const particles_block_t *block2)
{
	float *x = forces->x;
	float *y = forces->y;
	float *z = forces->z;

	const int same_block = (block1 == block2);
	const float *pos_x1 = block1->position_x;
	const float *pos_y1 = block1->position_y;
	const float *pos_z1 = block1->position_z;
	const float *mass1  = block1->mass ;

	const float *pos_x2 = block2->position_x;
	const float *pos_y2 = block2->position_y;
	const float *pos_z2 = block2->position_z;
	const float *mass2  = block2->mass;

	for (int i = 0; i < BLOCK_SIZE; i++) {
		float fx = x[i], fy = y[i], fz = z[i];
		for (int j = 0; j < BLOCK_SIZE; j++) {
			const float diff_x = pos_x2[j] - pos_x1[i];
			const float diff_y = pos_y2[j] - pos_y1[i];
			const float diff_z = pos_z2[j] - pos_z1[i];

			const float distance_squared = diff_x * diff_x + diff_y * diff_y + diff_z * diff_z;
			const float distance = sqrtf(distance_squared);

			float force = 0.0f;
			if (!same_block || distance_squared != 0.0f) {
				force = (mass1[i] / (distance_squared * distance)) * (mass2[j] * gravitational_constant);
			}
			fx += force * diff_x;
			fy += force * diff_y;
			fz += force * diff_z;
		}
		x[i] = fx;
		y[i] = fy;
		z[i] = fz;
	}
}

#pragma oss task label("update_particles_block") inout(*particles, *forces)
void update_particles_block(particles_block_t *particles, forces_block_t *forces, const float time_interval)
{
	for (int e = 0; e < BLOCK_SIZE; e++){
		const float mass       = particles->mass[e];
		const float velocity_x = particles->velocity_x[e];
		const float velocity_y = particles->velocity_y[e];
		const float velocity_z = particles->velocity_z[e];
		const float position_x = particles->position_x[e];
		const float position_y = particles->position_y[e];
		const float position_z = particles->position_z[e];

		const float time_by_mass       = time_interval / mass;
		const float half_time_interval = 0.5f * time_interval;

		const float velocity_change_x = forces->x[e] * time_by_mass;
		const float velocity_change_y = forces->y[e] * time_by_mass;
		const float velocity_change_z = forces->z[e] * time_by_mass;
		const float position_change_x = velocity_x + velocity_change_x * half_time_interval;
		const float position_change_y = velocity_y + velocity_change_y * half_time_interval;
		const float position_change_z = velocity_z + velocity_change_z * half_time_interval;

		particles->velocity_x[e] = velocity_x + velocity_change_x;
		particles->velocity_y[e] = velocity_y + velocity_change_y;
		particles->velocity_z[e] = velocity_z + velocity_change_z;
		particles->position_x[e] = position_x + position_change_x;
		particles->position_y[e] = position_y + position_change_y;
		particles->position_z[e] = position_z + position_change_z;
	}

	memset(forces, 0, sizeof(forces_block_t));
}

// TODO: Remove the serialization of communication tasks

#pragma oss task label("send_particles_block") in(*sendbuf) inout(serial)
void send_particles_block(const particles_block_t *sendbuf, int block_id, int dst)
{
	MPI_Send(sendbuf, sizeof(particles_block_t), MPI_BYTE, dst, block_id+10, MPI_COMM_WORLD);
}

#pragma oss task label("recv_particles_block") out(*recvbuf) inout(serial)
void recv_particles_block(particles_block_t *recvbuf, int block_id, int src)
{
    MPI_Recv(recvbuf, sizeof(particles_block_t), MPI_BYTE, src, block_id+10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void nbody_stats(const nbody_t *nbody, const nbody_conf_t *conf, double time)
{
	int rank, rank_size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &rank_size);

	if (!rank) {
		int particles = nbody->num_blocks * BLOCK_SIZE;
		int total_particles = particles * rank_size;

		printf("bigo, %s, processes, %d, threads, %d, timesteps, %d, total_particles, %d, particles_per_proc, %d, block_size, %d, blocks_per_proc, %d, time, %.2f, performance, %.2f\n",
			TOSTRING(BIGO), rank_size, nanos6_get_num_cpus(), nbody->timesteps, total_particles, particles, BLOCK_SIZE,
			nbody->num_blocks, time, nbody_compute_throughput(total_particles, nbody->timesteps, time)
		);
	}
}

int get_tagbase(int iteration, int num_blocks)
{
	// We define three different MPI tagbases depending on the
	// shift iteration
	// - First iteration: [num_blocks*2, num_blocks*3)
	// - Odd iterations:  [num_blocks, num_blocks*2)
	// - Even iterations: [0, num_blocks)
	if (iteration == 0) {
		return num_blocks * 2;
	} else if (iteration % 2) {
		return num_blocks;
	} else {
		return 0;
	}
}
