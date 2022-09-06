Cholesky kernel (MKL)
=====================

This example shows the Cholesky kernel. This algorithm is a decomposition of a
Hermitian, positive-definite matrix into the product of a lower triangular
matrix and its conjugate transpose.

.. highlight:: c
The kernel uses four different linear algorithms: potrf, trsm, gemm and syrk.
The following code shows the basic pattern for a Cholesky factorization::

   for (int k = 0; k < nt; k++) {
      // Diagonal Block factorization
      oss_potrf (Ah[k][k], ts, ts);

      // Triangular systems
      for (int i = k + 1; i < nt; i++)
         oss_trsm (Ah[k][k], Ah[k][i], ts, ts);

      // Update trailing matrix
      for (int i = k + 1; i < nt; i++) {
         for (int j = k + 1; j < i; j++)
            oss_gemm (Ah[k][i], Ah[k][j], Ah[j][i], ts, ts);

         oss_syrk (Ah[k][i], Ah[i][i], ts, ts);
      }

   }

In this case we parallelize the code by annotating the kernel functions.
So each call in the previous loop becomes the instantiation of a task.
The following code shows how we have parallelized Cholesky::

   #pragma oss task inout([ts][ts]A)
   void omp_potrf(double * const A, int ts, int ld)
   {
      ...
   }

   #pragma oss task in([ts][ts]A) inout([ts][ts]B)
   void omp_trsm(double *A, double *B, int ts, int ld)
   {
      ...
   }

   #pragma oss task in([ts][ts]A) inout([ts][ts]B)
   void omp_syrk(double *A, double *B, int ts, int ld)
   {
      ...
   }

   #pragma oss task in([ts][ts]A, [ts][ts]B) inout([ts][ts]C)
   void omp_gemm(double *A, double *B, double *C, int ts, int ld)
   {
      ...
   }

Note that for each of the dependences we also specify which is the matrix
(block) size.  Although this is not needed, due that there is no overlapping
among different blocks, it will allow the runtime to compute dependences
using the region mechanism.

Goals of this exercise
----------------------
* Code is completely annotated: you DON'T need to modify it. You only need to compile and execute.
* Review source code and check the different directives and their clauses. Try to understand what they mean.
* Pay special attention to data-sharing attributes, including those that do not appear (i.e., the implicit ones).
* Pay special attention also to directives being outlined in kernel functions.
* Check several runtime options when executing the program (versions, schedulers, etc.).
* Check scalability. Execute the program using different numbers of cpus and compute the speed-up.
* Change program arguments that may have an impact on task granularity (block size, tile size, etc.).
* Change program arguments that may have an impact on the number of tasks (matrix sizes and/or block/tile sizes).
* Get different paraver traces using different runtime options or program arguments and compare them.


Execution instructions
----------------------

You should run the program as::

  ./cholesky M T C

where:

* ``M`` is the size of the matrix to factorize (number of rows and columns, MxM).
* ``T`` is the tile size of the matrix.
* ``C`` specifies whether a correctness check must be done, in order to assure that the factorization was correct.
* The factorization will be applied in blocks that contain ``TxT`` elements.

References
----------
* https://en.wikipedia.org/wiki/Cholesky_decomposition
* https://en.wikipedia.org/wiki/Factorization


