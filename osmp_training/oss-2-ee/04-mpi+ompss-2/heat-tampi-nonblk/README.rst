Heat difussion (TAMPI, Non-blocking MPI Functions)
=============================

The Heat simulation uses an iterative Gauss-Seidel method to solve the heat
equation, which is a parabolic partial differential equation that describes the
distribution of heat (or variation in temperature) in a given region over time.
The heat equation is of fundamental importance in a wide range of science
fields. In mathematics, it is the parabolic partial differential equation par
excellence. In statistics, it is related to the study of the Brownian motion.
Also, the diffusion equation is a generic version of the heat equation, and it
is related to the study of chemical diffusion processes.

.. highlight:: c
This application has been parallelized using MPI and OmpSs-2. The MPI part
requires an MPI implementation supporting the multi-threading mode.  It mainly
targets the Intel MPI implementation, however, it should work with other libraries
by adding the needed implementation-specific flags in the Makefile.

A second level of parallelism is exploited following the OmpSs-2 approach.  The
program uses tasks to express the parallelism among different blocks.
Communication services have also been taskified but their execution has been
serialized by means of a sentinel dependence (in the code `serial`) that
guarantees the proper order of execution. The following functions are examples
of this serialization::

   inline void sendFirstComputeRow(block_t *matrix, int nbx, int nby, int rank, int rank_size)
   {
      for (int by = 1; by < nby-1; ++by) {
         #pragma oss task label(end first row) in(([nbx][nby]matrix)[1][by]) inout(serial)
         MPI_Send(&matrix[nby+by][0], BSY, MPI_DOUBLE, rank - 1, by, MPI_COMM_WORLD);
      }
   }
   
   inline void sendLastComputeRow(block_t *matrix, int nbx, int nby, int rank, int rank_size)
   {
      for (int by = 1; by < nby-1; ++by) {
         #pragma oss task label(send last row) in(([nbx][nby]matrix)[nbx-2][by]) inout(serial)
         MPI_Send(&matrix[(nbx-2)*nby + by][BSX-1], BSY, MPI_DOUBLE, rank + 1, by, MPI_COMM_WORLD);
      }
   }

The first step is to reproduce the version obtained in the previous exercise::

    Taking as a baseline this version of the program we want to exploit the TAMPI
    interoperability layer capabilities. The Makefile system already include the
    compilation and link against TAMPI (then, MPI services can be captured by this
    supporting library). We will need to make the proper changes in the code to
    inform the program will use a different level of thread support (where
    communication services potentially can become task switching points). Once we
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

The binaries accept several options. The most relevant options are the size of
the matrix in each dimension with `-s`, and the number of timesteps with `-t`.
More options can be seen passing the `-h` option. An example of execution could
be::

   mpiexec.hydra -n 4 ./heat -t 150 -s 8192

in which the application will perform 150 timesteps in 4 MPI processes, where
each process will have the same number of cores (# of available cores / 4). 
The size of the matrix in each dimension will be 8192 (8192 elements in total),
this means that each process will have 2048 * 8192 elements (16 blocks per process).
