
S=16777216
TS=8192

ITERS=100

#INSTRUMENT=./trace.sh
#INSTRUMENT=./graph.sh

$INSTRUMENT ./axpy $S $TS $ITERS
