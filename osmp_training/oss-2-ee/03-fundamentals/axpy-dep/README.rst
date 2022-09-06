Axpy (dep)
==========

This benchmark runs several AXPY operations. AXPY is a Level 1 operation in the
Basic Linear Algebra Subprograms (BLAS) package, and is a common operation in
computations with vector processors. AXPY is a combination of scalar
multiplication and vector addition.

.. highlight:: c
We want to parallelize the code using the ``task`` directive in order to
express parallelism but, similar to what we saw in the examples section, we
want to use fine-grained dependences (instead of taskwaits) to synchronize
across SAXPY operations. At some point we will probably need to synchronize and
wait for all the instantiated tasks Ideally we should focus our effort in the
following two functions::

   #pragma oss task
   void axpy_block(double *x, double *y, double alpha, long N) {
   	for (long i=0; i < N; ++i) {
   		y[i] += alpha * x[i];
   	}
   }
   
   void axpy(double *x, double *y, double alpha, long N, long TS) {
   	for (long i=0; i < N; i+=TS) {
   		axpy_block(x+i, y+i, alpha, std::min(TS, N-i));
   	}
   }

Note that we have initially selected the declaration of the ``axpy_block``
function as a candidate to become a task. We could also have annotated the call
to that function, as in the following code::

   void axpy(double *x, double *y, double alpha, long N, long TS) {
   	for (long i=0; i < N; i+=TS) {
         #pragma oss task
   		axpy_block(x+i, y+i, alpha, std::min(TS, N-i));
   	}
   }

Goals of this exercise
----------------------
* Try to understand what is doing each part of the code. Complete the annotation of the existant tasks.
* When creating tasks, pay special attention to data-sharing attributes.
* Include synchroniztion directives when required (e.g., you should no start the verification until all the computation has been executed).
* Check several runtime options when executing the program (versions, schedulers, etc.).
* Check scalability. Execute the program using different number of cpus and compute the speed-up.
* Change program's arguments that may impact on task granularity (block size, tile size, etc.).
* Change program's arguments that may impact on the number of tasks (matrix sizes and/or block/tile sizes).
* Get different paraver traces using different runtime options or program arguments and compare them.

Execution instructions
----------------------

You should run the program as::

  ./saxpy SIZE BLOCK_SIZE INTERATIONS

where:

* ``SIZE`` is the number of elements of the vectors used on the SAXPY operation.
* The SAXPY operation will be applied to the vector in blocks that contains ``BLOCK_SIZE`` elements.
* ``ITERATIONS`` is the number of times the SAXPY operation is executed.

References
----------
* http://en.wikipedia.org/wiki/AXPY

