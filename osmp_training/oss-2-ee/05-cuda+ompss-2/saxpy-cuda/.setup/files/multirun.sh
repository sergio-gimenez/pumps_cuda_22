
PROGRAM=multisaxpy_cuda

BLOCKSIZES=(4096 8192 16384)

for bs in ${BLOCKSIZES[@]}; do
	./$PROGRAM 16777216 $bs 10
done
