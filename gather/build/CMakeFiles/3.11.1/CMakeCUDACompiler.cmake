set(CMAKE_CUDA_COMPILER "/usr/local/cuda-9.1/bin/nvcc")
set(CMAKE_CUDA_HOST_COMPILER "")
set(CMAKE_CUDA_HOST_LINK_LAUNCHER "/apps/GCC/6.4.0/bin/g++")
set(CMAKE_CUDA_COMPILER_ID "NVIDIA")
set(CMAKE_CUDA_COMPILER_VERSION "9.1.85")
set(CMAKE_CUDA_STANDARD_COMPUTED_DEFAULT "14")
set(CMAKE_CUDA_SIMULATE_ID "")
set(CMAKE_CUDA_SIMULATE_VERSION "")


set(CMAKE_CUDA_COMPILER_ENV_VAR "CUDACXX")
set(CMAKE_CUDA_HOST_COMPILER_ENV_VAR "CUDAHOSTCXX")

set(CMAKE_CUDA_COMPILER_ID_RUN 1)
set(CMAKE_CUDA_SOURCE_FILE_EXTENSIONS cu)
set(CMAKE_CUDA_LINKER_PREFERENCE 15)
set(CMAKE_CUDA_LINKER_PREFERENCE_PROPAGATES 1)

set(CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES "/usr/local/cuda-9.1/targets/ppc64le-linux/include")

set(CMAKE_CUDA_HOST_IMPLICIT_LINK_LIBRARIES "cudadevrt;cudart_static;rt;pthread;dl")
set(CMAKE_CUDA_HOST_IMPLICIT_LINK_DIRECTORIES "/usr/local/cuda-9.1/targets/ppc64le-linux/lib/stubs;/usr/local/cuda-9.1/targets/ppc64le-linux/lib")
set(CMAKE_CUDA_HOST_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")

set(CMAKE_CUDA_IMPLICIT_LINK_LIBRARIES "cudadevrt;cudart_static;rt;pthread;dl;stdc++;m;gcc_s;gcc;c;gcc_s;gcc")
set(CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES "/usr/local/cuda-9.1/targets/ppc64le-linux/lib/stubs;/usr/local/cuda-9.1/targets/ppc64le-linux/lib;/gpfs/apps/POWER9/GCC/6.4.0/lib/gcc/ppc64le-redhat-linux/6.4.0;/gpfs/apps/POWER9/GCC/6.4.0/lib/gcc;/usr/local/cuda-9.1/extras/CUPTI/lib64;/usr/local/cuda-9.1/nvvm/lib64;/gpfs/apps/POWER9/GCC/6.4.0/lib64;/lib64;/usr/lib64;/usr/local/cuda-9.1/lib64;/apps/OPENMPI/3.0.0/GCC/lib;/gpfs/apps/POWER9/GCC/6.4.0/ppc64le-redhat-linux/lib;/gpfs/apps/POWER9/GCC/6.4.0/lib")
set(CMAKE_CUDA_IMPLICIT_LINK_FRAMEWORK_DIRECTORIES "")
