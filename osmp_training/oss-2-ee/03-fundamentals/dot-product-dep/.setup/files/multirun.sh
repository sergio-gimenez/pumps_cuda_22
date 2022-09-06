
export IFS=";"

S="262144"
TS="8192;2048;512;128"

for ts in $TS; do
  ./dot-product $S $ts 
done
