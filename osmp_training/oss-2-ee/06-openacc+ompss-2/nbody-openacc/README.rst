N-body benchmark with OpenACC
==============================

Nbody simulation is a simulation of a dynamical system of particles, usually under
the influence of physical forces, such as gravity.

Versions

* **01.nbody_openacc:** Parallel version using OpenACC tasks

Execution instructions

To run the program use:
```
./nbody_openacc [input-file]
```

Input files can be found in the *data/input* folder. 
The programs creates a *nbody_out.xyz* file which can be compared with the reference file in the *data/output* folder to check for correctness.
Note that due to floating point precision you may get slightly different results in certain lines of the output. Use a tool 
like `diff` or `vimdiff` to inspect and figure out if the differences are actually negligible.
