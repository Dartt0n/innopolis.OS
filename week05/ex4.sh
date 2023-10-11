#!/bin/bash

OUT=./main

gcc ex4.c -lm -pthread -o $OUT

rm -rf ex4_res.txt
for m in {1,2,4,10,100}
do
    echo "m = $m"  >> ex4_res.txt
    { time $OUT 10000000 $m; } 2>> ex4_res.txt
    echo "############" >> ex4_res.txt
done

rm $OUT
