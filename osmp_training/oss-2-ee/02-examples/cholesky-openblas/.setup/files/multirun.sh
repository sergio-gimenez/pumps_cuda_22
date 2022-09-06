
export IFS=";"

MSIZES="4096;2048"
BSIZES="512;256"

for MS in $MSIZES; do
  for BS in $BSIZES; do
    ./cholesky $MS $BS 0
  done
done
