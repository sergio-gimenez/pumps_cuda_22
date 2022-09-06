
SIZES=(4096 8192 16384)
TIMESTEPS=100

for size in ${SIZES[@]}; do
	$OSS2EE_MPIRUN_COMMAND ./nbody -p $size -t $TIMESTEPS
done
