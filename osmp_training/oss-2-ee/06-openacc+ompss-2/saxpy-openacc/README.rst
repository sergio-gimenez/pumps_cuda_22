Saxpy-OpenACC benchmark
=======================

Description

This benchmark runs several SAXPY operations in the GPU.
SAXPY is a Level 1 operation in the Basic Linear Algebra Subprograms package,
and is a common operation in computations with vector processors. SAXPY is a
combination of scalar multiplication and vector addition.

Versions

* **01.multisaxpy_openacc:** Parallel version using OpenACC tasks

Execution instructions

To run the program use:
```
./multisaxpy_openacc SIZE BLOCK_SIZE ITERATIONS
```

where:

1. SIZE is the number of elements of the vectors used on the SAXPY operation
2. The SAXPY operation will be applied to the vector in blocks that contains BLOCK_SIZE elements
3. ITERATIONS is the number of times the SAXPY operation is executed

References
http://en.wikipedia.org/wiki/AXPY

