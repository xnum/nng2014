#!/bin/bash

for file in TAAI*-question.txt
do ./main -I $file -L ${file:0:8}.log --simple-stdout --show-config --yes-log
done

