N-body benchmark (TAMPI, Non-blocking MPI Functions)
================

An N-body simulation numerically approximates the evolution of a system of
bodies in which each body continuously interacts with every other body.  A
familiar example is an astrophysical simulation in which each body represents a
galaxy or an individual star, and the bodies attract each other through the
gravitational force.

N-body simulation arises in many other computational science problems as well.
For example, protein folding is studied using N-body simulation to calculate
electrostatic and van der Waals forces. Turbulent fluid flow simulation and
global illumination computation in computer graphics are other examples of
problems that use N-body simulation.

.. highlight:: c
This application has been parallelized using MPI and OmpSs-2. The MPI part
requires an MPI implementation supporting the multi-threading mode.  It mainly
targets the Intel MPI implementation, however, it should work with other libraries
when adding the needed implementation-specific flags in the Makefile.

A second level of parallelism is exploited following the OmpSs-2 approach.  The
program uses tasks to express the parallelism among the different blocks.
Communication services have also been taskified but their execution has been
serialized by means of a sentinel dependence (in the code `serial`) that
guarantees the proper order of execution. The following function centralizes
all the communication operations to exchange data among processes::

   #pragma oss task label(send_particles_block) in(*sendbuf) inout(serial)
   void send_particles_block(const particles_block_t *sendbuf, int block_id, int dst)
   {
      MPI_Send(sendbuf, sizeof(particles_block_t), MPI_BYTE, dst, block_id+10, MPI_COMM_WORLD);
   }
   
   #pragma oss task label(recv_particles_block) out(*recvbuf) inout(serial)
   void recv_particles_block(particles_block_t *recvbuf, int block_id, int src)
   {
      MPI_Recv(recvbuf, sizeof(particles_block_t), MPI_BYTE, src, block_id+10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
   }

The first step is to reproduce the version obtained in the previous exercise::

    Taking as a baseline this version of the program we want to exploit the TAMPI
    interoperability layer capabilities. The Makefile system already includes the
    compilation and linkage against TAMPI (then, MPI services can be captured by this
    supporting library). We will need to make the proper changes in the code to
    inform the program about the usage of a different level of thread support (where
    communication services can potentially become task switching points). Once we
    have this thread support level we can relax task execution order by removing
    artificial dependences.

Then we can go one step further and transform our blocking service calls into
their non-blocking counterparts. Don't forget to wait for the completion of
communication before finalizing communication tasks.

Goals of this exercise
----------------------
* Transform the code in order to exploit TAMPI capabilities for non-blocking communications. Look for TODO comments.
* Compare performance results between the initial version and your candidate version.
* You can also compare the results obtained in the blocking communication exercise.
* Check several runtime options when executing the program (versions, schedulers, etc.).
* Check scalability. Execute the program using different numbers of cpus or MPI processes and compute the speed-up.
* Change program arguments that may have an impact on task granularity (block size, tile size, etc.).
* Change program arguments that may have an impact on the number of tasks (matrix sizes and/or block/tile sizes).
* Get different paraver traces using different runtime options or program arguments and compare them.

Execution instructions
----------------------

The binaries accept several options. The most relevant options are the number
of total particles with `-p`, and the number of timesteps with `-t`. More
options can be seen passing the `-h` option. An example of execution could be::

   $ mpiexec.hydra -n 4 ./nbody -t 100 -p 8192

in which the application will perform 100 timesteps in 4 MPI processes, where
each process will have the same number of cores (# of available cores / 4).
The total number of particles will be 8192, this means that each process will
have 2048 particles (2 blocks per process).

