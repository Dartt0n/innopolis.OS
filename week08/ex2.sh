#!/bin/bash

gcc mmu.c -o mmu
gcc pager.c -o pager

echo
echo
echo "########################"
echo "#        TEST 1        #"
echo "#______________________#"
echo "#      P=4    F=2      #"
echo "########################"
echo
echo

./pager 4 2 &
pid=$!
sleep 0.1
./mmu 4 "R0 R1 W1 R0 R2 W2 R0 R3 W2" $pid

sleep 2

echo
echo
echo "########################"
echo "#        TEST 2        #"
echo "#______________________#"
echo "#      P=5    F=3      #"
echo "########################"
echo
echo

./pager 5 3 &
pid=$!
sleep 0.1
./mmu 5 "R0 R1 R0 W1 R0 R1 R0 W1 R0 R2 R0 W2 R0 R2 R0 W2 R0 R3 R0 W3 R0 R3 R0 W3 R0 R4 R0 W4 R0 R4 R0 W4" $pid

rm mmu pager