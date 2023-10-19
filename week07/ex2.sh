#!/bin/bash

set -e

rm -rf text.txt

gcc ex2.c -o ex2
./ex2

rm ex2