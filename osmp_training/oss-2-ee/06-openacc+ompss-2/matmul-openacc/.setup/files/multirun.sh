PROGRAM=matmul_openacc

# Run non-tiled

./$PROGRAM

# Run tiled

./$PROGRAM 1 8
