#!/bin/bash

gcc ex1.c -o ex1
gcc monitor.c -o monitor
mkdir test

echo "Hello world!" > test/orig.txt


rm ex1 monitor
rm -rf test

