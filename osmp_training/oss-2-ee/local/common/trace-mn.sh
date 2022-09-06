#!/bin/bash

# Uncomment the following line to trace MPI+OmpSs-2 programs
export LD_PRELOAD=${EXTRAE_HOME}/lib/libnanosmpitrace.so

# Uncomment the following line to trace MPI+OpenMP (GNU) programs
#export LD_PRELOAD=${EXTRAE_HOME}/lib/libompitrace.so

export EXTRAE_CONFIG_FILE=extrae.xml
export NANOS6_CONFIG_OVERRIDE="$NANOS6_CONFIG_OVERRIDE,version.instrument=extrae"

$*
