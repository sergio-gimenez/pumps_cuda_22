#ifndef NBODY_H
#define NBODY_H

#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

// BIGO definition
#ifndef BIGO
#define BIGO N2
#define _BIGO_N2
#endif

// Block size definition
#ifndef BLOCK_SIZE
#define BLOCK_SIZE 1024
#endif

#define MIN_PARTICLES (4096 * BLOCK_SIZE / sizeof(particles_block_t))

#define PART 1024
#define PAGE_SIZE 4096

#define TOLERATED_ERROR 0.0008

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define LOG2(a) (31-__builtin_clz((a)))
#define MOD(a, b)  ((a) < 0 ? ((((a) % (b)) + (b)) % (b)) : ((a) % (b)))
#define ROUNDUP(x, y) ({             \
    const typeof(y) __y = y;         \
    (((x) + (__y - 1)) / __y) * __y; \
})

#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)
#define CALCULATE_FORCES(s) calculate_forces_##s
#define XCALCULATE_FORCES(s) CALCULATE_FORCES(s)
#define calculate_forces XCALCULATE_FORCES(BIGO)

static const float gravitational_constant = 6.6726e-11f; /* N(m/kg)2 */
static const float default_domain_size_x  = 1.0e+10; /* m  */
static const float default_domain_size_y  = 1.0e+10; /* m  */
static const float default_domain_size_z  = 1.0e+10; /* m  */
static const float default_mass_maximum   = 1.0e+28; /* kg */
static const float default_time_interval  = 1.0e+0;  /* s  */
static const int   default_seed           = 12345;
static const char* default_name           = "data/nbody";
static const int   default_num_particles  = 0;
static const int   default_timesteps      = 0;
static const int   default_save_result    = 0;
static const int   default_check_result   = 0;

// Solver structures
typedef struct {
	float position_x[BLOCK_SIZE]; /* m   */
	float position_y[BLOCK_SIZE]; /* m   */
	float position_z[BLOCK_SIZE]; /* m   */
	float velocity_x[BLOCK_SIZE]; /* m/s */
	float velocity_y[BLOCK_SIZE]; /* m/s */
	float velocity_z[BLOCK_SIZE]; /* m/s */
	float mass[BLOCK_SIZE];       /* kg  */
	float weight[BLOCK_SIZE];
} particles_block_t;

typedef struct {
	float x[BLOCK_SIZE]; /* x   */
	float y[BLOCK_SIZE]; /* y   */
	float z[BLOCK_SIZE]; /* z   */
} forces_block_t;

// Application structures
typedef struct {
	size_t total_size;
	size_t size;
	size_t offset;
	char name[1000];
} nbody_file_t;

typedef struct {
	particles_block_t *local;
	particles_block_t *remote1;
	particles_block_t *remote2;
	forces_block_t *forces;
	int num_blocks;
	int timesteps;
	nbody_file_t file;
} nbody_t;

typedef struct {
	float domain_size_x;
	float domain_size_y;
	float domain_size_z;
	float mass_maximum;
	float time_interval;
	int seed;
	const char* name;
	int num_particles;
	int num_blocks;
	int timesteps;
	int save_result;
	int check_result;
} nbody_conf_t;


// Solver function
void nbody_solve(nbody_t *nbody, const int num_blocks, const int timesteps, const float time_interval);

// Auxiliary functions
nbody_t nbody_setup(const nbody_conf_t *conf);
void nbody_particle_init(const nbody_conf_t *conf, particles_block_t *part);
void nbody_stats(const nbody_t *nbody, const nbody_conf_t *conf, double time);
void nbody_save_particles(const nbody_t *nbody);
void nbody_free(nbody_t *nbody);
void nbody_check(const nbody_t *nbody);
int nbody_compare_particles(const particles_block_t *local, const particles_block_t *reference, int num_blocks);
nbody_conf_t nbody_get_conf(int argc, char **argv);
double nbody_compute_throughput(int num_particles, int timesteps, double elapsed_time);
void * nbody_alloc(size_t size);
double get_time();


#endif // NBODY_H

