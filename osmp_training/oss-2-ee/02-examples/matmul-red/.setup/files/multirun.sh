
export IFS=";"

MSIZE="2048"
NSIZE="4096"
PSIZE="1024"

BSIZES="512;256"

# This program needs support for region dependencies
export NANOS6_CONFIG_OVERRIDE="version.dependencies=regions"

for BS in $BSIZES; do
  ./matmul $MSIZE $NSIZE $PSIZE $BS 
done
