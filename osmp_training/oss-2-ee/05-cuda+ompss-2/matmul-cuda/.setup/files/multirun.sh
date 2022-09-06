PROGRAM=matmul-p

SIZES=(8192 16384)

for size in ${SIZES[@]}; do
	touch test.in
	echo "$size $size $size 5" > test.in
	./$PROGRAM
done

