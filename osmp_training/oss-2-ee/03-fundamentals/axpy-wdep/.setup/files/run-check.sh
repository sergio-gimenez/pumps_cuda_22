#!/bin/bash

export IFS=";"

S="16777216"
TS="8192;4096"

ITERS="100"

for ts in $TS; do
   CHECK_OPTIONS="ts=$ts" oss-check ./axpy $S $ts $ITERS
done
