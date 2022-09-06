Matrix multiplication (dep)
===========================

This benchmark runs a matrix multiplication operation C = AB, where A has size
NxM, B has size MxP, and the resulting matrix C has size NxP.  The kernel uses
a blocking version of the matrix multiplication algorithm with three nested
loops chasing the matrix multiplication index pattern; the loop body implements
the inner/block matrix multiplication.

.. highlight:: c
The parallelization approach will use the task dependence clauses in order
to postpone the need of the ``taskwait`` directive::

   for (long k = 0; k < NB_K; k++) {
      for (long i = 0; i < NB_I; i++) {
         #pragma oss task in([N/TS]A) in([M/TS]B) inout([N/TS]C) label(matmul block)
         for (long j = 0; j < NB_J; j++) {
            double (* restrict A_block)[TS] = A[i][k];
            double (* restrict B_block)[TS] = B[k][j];
            double (* restrict C_block)[TS] = C[i][j];
   
            for (long ii = 0; ii < TS; ii++) {
               for (long jj = 0; jj < TS; jj++) {
                  for (long kk = 0; kk < TS; kk++) {
                     C_block[ii][jj] += A_block[ii][kk] * B_block[kk][jj];
                  }
               }
            }
         }
      }
   }
   #pragma oss taskwait

The parallelization creates a task containing the 'j' loop (i.e. the 'i' loop
body), each one annotated with the correspondent data directionality clauses
(``in``, ``out`` and ``inout``).  That mechanism guarantees the proper
synchronization among all the instantiated tasks for a given matrix
multiplication.  After all the 'k' iterations (the whole matrix multiplication)
the code executes a taskwait directive which guarantees the execution of all
the previous instantiated tasks before continuing.  The algorithm opens and
closes the parallelism only once per matrix multiplication computation.

Goals of this exercise
----------------------
* Code is completely annotated: you DON'T need to modify it. You only need to compile and execute.
* Review source code and check the different directives and their clauses. Try to understand what they mean.
* Pay special attention to data-sharing attributes, including those that do not appear (i.e., the implicit ones).
* Check several runtime options when executing the program (versions, schedulers, etc.).
* Check scalability. Execute the program using different numbers of cpus and compute the speed-up.
* Change program arguments that may have an impact on task granularity (block size, tile size, etc.).
* Change program arguments that may have an impact on the number of tasks (matrix sizes and/or block/tile sizes).
* Get different paraver traces using different runtime options or program arguments and compare them.


Execution instructions
----------------------

You should run the program as::

  ./matmul N M P TS

where:

* ``N`` is the number of rows of the matrix A.
* ``M`` is the number of columns of the matrix A and the number of rows of the matrix B.
* ``P`` is the number of columns of the matrix B.
* The matrix multiplication operation will be applied in blocks that contain ``TSxTS`` elements.

References
----------
* https://en.wikipedia.org/wiki/Matrix_multiplication
* https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm
