
export IFS=";"

S="262144"
TS="8192;4096;2048"

for ts in $TS; do
  ./dot-product $S $ts 
done
