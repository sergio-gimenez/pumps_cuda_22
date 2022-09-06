#include <cuda_runtime_api.h>

void *allocate(size_t size)
{
#ifdef OSS2EE_NANOS6_DIRECTORY
   // Allocate regular memory
   return malloc(size);
#else
   // Allocate managed memory
   void *data;
   cudaMallocManaged((void **) &data, size, cudaMemAttachGlobal);
   return data;
#endif
}

void deallocate(void *data)
{
#ifdef OSS2EE_NANOS6_DIRECTORY
   free(data);
#else
   cudaFree(data);
#endif
}
