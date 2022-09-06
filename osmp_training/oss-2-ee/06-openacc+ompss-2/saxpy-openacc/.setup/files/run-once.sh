
PROGRAM=multisaxpy_openacc

#INSTRUMENT=./trace.sh
#INSTRUMENT=./graph.sh

$INSTRUMENT ./$PROGRAM 16777216 8192 10
