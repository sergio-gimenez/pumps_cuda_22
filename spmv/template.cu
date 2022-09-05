#include <stdio.h>
#include <chrono>
#include <algorithm>
#include <cuda.h>
#include <iostream>

__global__ void
spmvCSRKernel(float *out, int *matCols, int *matRows, float *matData, float *vec, int dim)
{
    int outIdx   = blockDim.x * blockIdx.x + threadIdx.x;
    float outReg = 0;
    int start, end, i;
    if (outIdx < dim) {
        outReg = 0;
        start  = matRows[outIdx];
        end    = matRows[outIdx + 1];
        for (i = start; i < end; i++) {
            outReg += matData[i] * vec[matCols[i]];
        }
        out[outIdx] += outReg;
    }
}

__global__ void
spmvJDSKernel(float *out, int *matColStart, int *matCols, int *matRowPerm, int *matRows, float *matData, float *vec,
              int dim)
{
    int outIdx = blockDim.x * blockIdx.x + threadIdx.x;
    int prod, start, end, iterator;
    if (outIdx < dim) {
        prod  = 0;
        start = 0;
        end   = matRows[outIdx];
        for (iterator = start; iterator < end; iterator++) {
            prod += matData[matColStart[iterator] + outIdx] * vec[matCols[matColStart[iterator] + outIdx]];
        }
        out[matRowPerm[outIdx]] += prod;
    }
}

static void
spmvCSR(float *out, int *matCols, int *matRows, float *matData, float *vec, int dim)
{
    dim3 DimGrid(ceil((dim - 1) / 8.0), 1, 1);
    dim3 DimBlock(8, 1, 1);
    spmvCSRKernel<<<DimGrid, DimBlock>>>(out, matCols, matRows, matData, vec, dim);
}

static void
spmvJDS(float *out, int *matColStart, int *matCols, int *matRowPerm, int *matRows, float *matData, float *vec, int dim)
{
    dim3 DimGrid(ceil((dim - 1) / 8.0), 1, 1);
    dim3 DimBlock(8, 1, 1);

    spmvJDSKernel<<<DimGrid, DimBlock>>>(out, matColStart, matCols, matRowPerm, matRows, matData, vec, dim);
}

static void
sort(int *data, int *key, int start, int end)
{
    if ((end - start + 1) > 1) {
        int left = start, right = end;
        int pivot = key[right];
        while (left <= right) {
            while (key[left] > pivot) {
                left = left + 1;
            }
            while (key[right] < pivot) {
                right = right - 1;
            }
            if (left <= right) {
                int tmp     = key[left];
                key[left]   = key[right];
                key[right]  = tmp;
                tmp         = data[left];
                data[left]  = data[right];
                data[right] = tmp;
                left        = left + 1;
                right       = right - 1;
            }
        }
        sort(data, key, start, right);
        sort(data, key, left, end);
    }
}

void
CSRToJDS(int dim, int *csrRowPtr, int *csrColIdx, float *csrData, int **jdsRowPerm, int **jdsRowNNZ,
         int **jdsColStartIdx, int **jdsColIdx, float **jdsData)
{
    // Row Permutation Vector
    *jdsRowPerm = (int *)malloc(sizeof(int) * dim);
    for (int rowIdx = 0; rowIdx < dim; ++rowIdx) {
        (*jdsRowPerm)[rowIdx] = rowIdx;
    }

    // Number of non-zeros per row
    *jdsRowNNZ = (int *)malloc(sizeof(int) * dim);
    for (int rowIdx = 0; rowIdx < dim; ++rowIdx) {
        (*jdsRowNNZ)[rowIdx] = csrRowPtr[rowIdx + 1] - csrRowPtr[rowIdx];
    }

    // Sort rows by number of non-zeros
    sort(*jdsRowPerm, *jdsRowNNZ, 0, dim - 1);

    // Starting point of each compressed column
    int maxRowNNZ = (*jdsRowNNZ)[0]; // Largest number of non-zeros per row
    printf("jdsRowNNZ = %d\n", maxRowNNZ);
    *jdsColStartIdx      = (int *)malloc(sizeof(int) * maxRowNNZ);
    (*jdsColStartIdx)[0] = 0; // First column starts at 0
    for (int col = 0; col < maxRowNNZ - 1; ++col) {
        // Count the number of rows with entries in this column
        int count = 0;
        for (int idx = 0; idx < dim; ++idx) {
            if ((*jdsRowNNZ)[idx] > col) {
                ++count;
            }
        }
        (*jdsColStartIdx)[col + 1] = (*jdsColStartIdx)[col] + count;
    }

    // Sort the column indexes and data
    const int NNZ = csrRowPtr[dim];
    printf("NNZ = %d\n", NNZ);
    *jdsColIdx = (int *)malloc(sizeof(int) * NNZ);
    printf("dim = %d\n", dim);
    *jdsData = (float *)malloc(sizeof(float) * NNZ);
    for (int idx = 0; idx < dim; ++idx) { // For every row
        int row    = (*jdsRowPerm)[idx];
        int rowNNZ = (*jdsRowNNZ)[idx];
        for (int nnzIdx = 0; nnzIdx < rowNNZ; ++nnzIdx) {
            int jdsPos           = (*jdsColStartIdx)[nnzIdx] + idx;
            int csrPos           = csrRowPtr[row] + nnzIdx;
            (*jdsColIdx)[jdsPos] = csrColIdx[csrPos];
            (*jdsData)[jdsPos]   = csrData[csrPos];
        }
    }
}

int
main(int argc, char **argv)
{
    bool usingJDSQ;
    int *hostCSRCols;
    int *hostCSRRows;
    float *hostCSRData;
    int *hostJDSColStart;
    int *hostJDSCols;
    int *hostJDSRowPerm;
    int *hostJDSRows;
    float *hostJDSData;
    float *hostVector;
    float *hostOutput;
    int *deviceCSRCols;
    int *deviceCSRRows;
    float *deviceCSRData;
    int *deviceJDSColStart;
    int *deviceJDSCols;
    int *deviceJDSRowPerm;
    int *deviceJDSRows;
    float *deviceJDSData;
    float *deviceVector;
    float *deviceOutput;
    int dim, ncols, nrows, ndata;
    int maxRowNNZ;

    printf("Dataset Folder %s\n", argv[1]);
    printf("Importing data and creating memory on host\n");
    char filename[80];
    sprintf(filename, "%smode.flag", argv[1]);
    printf("%s\n", filename);
    FILE *fmode = fopen(filename, "r");
    fscanf(fmode, "%d/n", &usingJDSQ);
    fclose(fmode);
    sprintf(filename, "%scol.raw", argv[1]);
    printf("%s\n", filename);
    FILE *fcol = fopen(filename, "r");
    fscanf(fcol, "%d\n", &ncols);
    hostCSRCols = new int[ncols];
    for (int n = 0; n < ncols; n++)
        fscanf(fcol, "%d\n", hostCSRCols + n);
    fclose(fcol);
    sprintf(filename, "%srow.raw", argv[1]);
    printf("%s\n", filename);
    FILE *frow = fopen(filename, "r");
    fscanf(frow, "%d\n", &nrows);
    hostCSRRows = new int[nrows];
    for (int n = 0; n < nrows; n++)
        fscanf(frow, "%d\n", hostCSRRows + n);
    fclose(frow);
    sprintf(filename, "%sdata.raw", argv[1]);
    printf("%s\n", filename);
    FILE *fdata = fopen(filename, "r");
    fscanf(fdata, "%d\n", &ndata);
    hostCSRData = new float[ndata];
    for (int n = 0; n < ndata; n++)
        fscanf(fdata, "%e\n", hostCSRData + n);
    fclose(fdata);
    sprintf(filename, "%svec.raw", argv[1]);
    printf("%s\n", filename);
    FILE *fvec = fopen(filename, "r");
    fscanf(fvec, "%d\n", &dim);
    hostVector = new float[dim];
    for (int n = 0; n < dim; n++)
        fscanf(fvec, "%e\n", hostVector + n);
    fclose(fvec);
    if (usingJDSQ)
        printf("JDS Multiplication\n");
    else
        printf("CSR Multiplication\n");
    printf("#Columns: %d, #Rows: %d, #Data: %d, Dim: %d\n", ncols, nrows, ndata, dim);

    /*printf("hostCSRRows\n");
    for(int n = 0; n < nrows; n++)
      printf("%d\n",hostCSRRows[n]);
    printf("hostCSRCols\n");
    for(int n = 0; n < ncols; n++)
      printf("%d\n",hostCSRCols[n]);
    printf("hostVector\n");
    for(int n = 0; n < dim; n++)
      printf("%e\n",hostVector[n]);

    float *mat = new float[dim*dim];
    for(int n = 0; n < dim*dim; n++)
      mat[n] = 0;
    for(int m = 0; m < dim; m++)
      for(int n = hostCSRRows[m]; n < hostCSRRows[m+1]; n++)
        mat[m*dim+hostCSRCols[n]] = hostCSRData[n];
    printf("\n");
    for(int m = 0; m < dim; m++){
      for(int n = 0; n < dim; n++)
        printf("%.0f ",mat[m*dim+n]);
      printf("\n");
    }*/

    hostOutput = (float *)malloc(sizeof(float) * dim);

    if (usingJDSQ) {
        CSRToJDS(dim, hostCSRRows, hostCSRCols, hostCSRData, &hostJDSRowPerm, &hostJDSRows, &hostJDSColStart,
                 &hostJDSCols, &hostJDSData);
        maxRowNNZ = hostJDSRows[0];
    }

    printf("Allocating GPU memory.\n");
    if (usingJDSQ) {
        cudaMalloc((void **)&deviceJDSColStart, sizeof(int) * maxRowNNZ);
        cudaMalloc((void **)&deviceJDSCols, sizeof(int) * ndata);
        cudaMalloc((void **)&deviceJDSRowPerm, sizeof(int) * dim);
        cudaMalloc((void **)&deviceJDSRows, sizeof(int) * dim);
        cudaMalloc((void **)&deviceJDSData, sizeof(float) * ndata);
    } else {
        cudaMalloc((void **)&deviceCSRCols, sizeof(int) * ncols);
        cudaMalloc((void **)&deviceCSRRows, sizeof(int) * nrows);
        cudaMalloc((void **)&deviceCSRData, sizeof(float) * ndata);
    }
    cudaMalloc((void **)&deviceVector, sizeof(float) * dim);
    cudaMalloc((void **)&deviceOutput, sizeof(float) * dim);

    printf("Copying input memory to the GPU.\n");
    if (usingJDSQ) {
        cudaMemcpy(deviceJDSColStart, hostJDSColStart, sizeof(int) * maxRowNNZ, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceJDSCols, hostJDSCols, sizeof(int) * ndata, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceJDSRowPerm, hostJDSRowPerm, sizeof(int) * dim, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceJDSRows, hostJDSRows, sizeof(int) * dim, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceJDSData, hostJDSData, sizeof(float) * ndata, cudaMemcpyHostToDevice);
    } else {
        cudaMemcpy(deviceCSRCols, hostCSRCols, sizeof(int) * ncols, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceCSRRows, hostCSRRows, sizeof(int) * nrows, cudaMemcpyHostToDevice);
        cudaMemcpy(deviceCSRData, hostCSRData, sizeof(float) * ndata, cudaMemcpyHostToDevice);
    }
    cudaMemcpy(deviceVector, hostVector, sizeof(float) * dim, cudaMemcpyHostToDevice);

    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double> Duration;
    printf("Performing CUDA computation\n");
    auto start = Clock::now();
    if (usingJDSQ) {
        spmvJDS(deviceOutput, deviceJDSColStart, deviceJDSCols, deviceJDSRowPerm, deviceJDSRows, deviceJDSData,
                deviceVector, dim);
    } else {
        spmvCSR(deviceOutput, deviceCSRCols, deviceCSRRows, deviceCSRData, deviceVector, dim);
    }
    cudaDeviceSynchronize();
    Duration elapsed = Clock::now() - start;
    std::cout << elapsed.count() << " seconds\n";

    printf("Copying output memory to the CPU\n");
    cudaMemcpy(hostOutput, deviceOutput, sizeof(float) * dim, cudaMemcpyDeviceToHost);

    printf("Freeing GPU Memory\n");
    cudaFree(deviceCSRCols);
    cudaFree(deviceCSRRows);
    cudaFree(deviceCSRData);
    cudaFree(deviceVector);
    cudaFree(deviceOutput);
    if (usingJDSQ) {
        cudaFree(deviceJDSColStart);
        cudaFree(deviceJDSCols);
        cudaFree(deviceJDSRowPerm);
        cudaFree(deviceJDSRows);
        cudaFree(deviceJDSData);
    }

    sprintf(filename, "%soutput.raw", argv[1]);
    printf("%s\n", filename);
    FILE *fout = fopen(filename, "r");
    fscanf(fout, "%d\n", &dim);
    printf("dim: %d\n", dim);
    float *hostOut = new float[dim];
    for (int n = 0; n < dim; n++)
        fscanf(fout, "%e\n", &hostOut[n]);
    fclose(fout);

    bool res = true;
    for (int n = 0; n < dim; n++)
        if (hostOutput[n] != hostOut[n]) {
            printf("%e %e does not match at row %d!\n", hostOutput[n], hostOut[n], n);
            res = false;
        }
    if (res)
        printf("Result is correct!\n");
    else
        printf("some elements do not match\n");

    free(hostCSRCols);
    free(hostCSRRows);
    free(hostCSRData);
    free(hostVector);
    free(hostOutput);
    if (usingJDSQ) {
        free(hostJDSColStart);
        free(hostJDSCols);
        free(hostJDSRowPerm);
        free(hostJDSRows);
        free(hostJDSData);
    }

    return 0;
}
