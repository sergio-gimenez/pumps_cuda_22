
MSIZE="2048"
NSIZE="4096"
PSIZE="1024"
BSIZE="256"

# This program needs support for region dependencies
export NANOS6_CONFIG_OVERRIDE="version.dependencies=regions"

#INSTRUMENT=./trace.sh
#INSTRUMENT=./graph.sh

$INSTRUMENT ./matmul $MSIZE $NSIZE $PSIZE $BSIZE

