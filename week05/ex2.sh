#!/bin/bash

OUT=./main

gcc ex2.c -lm -pthread -o $OUT
$OUT 10
rm $OUT
