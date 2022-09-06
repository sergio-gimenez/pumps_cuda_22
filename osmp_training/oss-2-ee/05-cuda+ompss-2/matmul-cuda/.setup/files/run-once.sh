PROGRAM=matmul-p

# Creating the input file
touch test.in
echo "8192 8192 8192 5" > test.in

#INSTRUMENT=./trace.sh
#INSTRUMENT=./graph.sh

# Executing the application
$INSTRUMENT ./$PROGRAM
