#!/bin/bash

for file in TAAI*-question.txt
do
	for np in 11
	do 
		#./main -I $file -L ${file:0:8}.log --simple-stdout --show-config --yes-log
		echo "Run " $file " ON " $np " procs"
		mpirun -np $np ./main -I $file >${file:0:8}.${np}.5log 2>&1
	done
done
