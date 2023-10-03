#!/bin/bash

set -e

OUT=./main
gcc ex3.c -o $OUT

rm -f ex3-3.txt ex3-5.txt

$OUT 3 &
MPID=$!

echo "3" >> ex3-3.txt
pstree $MPID >> ex3-3.txt 
for I in {1..15}
do
    sleep 1
    pstree $MPID >> ex3-3.txt 
done

$OUT 5 &
MPID=$!

echo "5" >> ex3-5.txt
pstree $MPID >> ex3-5.txt 
for J in {1..25}
do
    sleep 1
    pstree $MPID >> ex3-5.txt 
done

rm $OUT
