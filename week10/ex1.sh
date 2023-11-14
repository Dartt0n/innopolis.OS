#!/bin/bash

gcc ex1.c -o ex1
gcc monitor.c -o monitor
mkdir test

./monitor test &
pid=$!

./ex1 test

sleep 1
kill -9 $pid

rm ex1 monitor
rm -rf test

