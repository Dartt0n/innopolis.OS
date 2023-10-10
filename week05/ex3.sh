#!/bin/bash

OUT=./main

gcc ex3.c -lm -pthread -o $OUT

rm -rf ex3_res.txt
for m in {1,2,4,10,100}
do
    echo "m = $m"  >> ex3_res.txt
    { time $OUT 10000000 $m; } 2>> ex3_res.txt
    echo "############" >> ex3_res.txt
done

rm $OUT
