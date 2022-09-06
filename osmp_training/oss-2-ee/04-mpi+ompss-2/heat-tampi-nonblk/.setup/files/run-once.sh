
PROGRAM=heat
PARAMS="-s 4096 -t 100"

#INSTRUMENT=./trace.sh
#INSTRUMENT=./graph.sh

$OSS2EE_MPIRUN_COMMAND $INSTRUMENT ./$PROGRAM $PARAMS

# Generate the trace if needed
if [[ "$INSTRUMENT" == *"trace"* ]]; then
	mpi2prv -f TRACE.mpits -o myTrace.prv
fi
