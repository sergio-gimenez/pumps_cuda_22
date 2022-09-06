
SIZES=(2048 4096 8192)
TIMESTEPS=100

for size in ${SIZES[@]}; do
	$OSS2EE_MPIRUN_COMMAND ./heat -s $size -t $TIMESTEPS
done
