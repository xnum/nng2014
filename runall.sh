#!/bin/bash

for file in *-question.txt; do ./main -I $file --simple-stdout ; done

