# Recompile the analyze.o file
printf "\n\n===== Recompiling analyze.o =====\n\n"
cd ../analyze
make clear
make analyze.o
cp analyze.o ../run/libs
cd ../run


# Run analyze
printf "\n\n===== Running analyze =====\n\n"

ARGV0=$0 # First argument is shell command (as in C)
# echo "Command: $ARGV0"

ARGC=$#  # Number of args, not counting $0
echo "Number of files: $ARGC"

i=1  # Used as argument index
while [ $i -le $ARGC ]; do # "-le" means "less or equal"
	# "${!i} "expands" (resolves) to $1, $2,.. by first expanding i, and
	# using the result (1,2,3..) as a variable which is then expanded.
	libs/analyze.o ${!i}
	i=$((i+1))
done
printf "Done.\n\n"
