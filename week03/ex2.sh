#!/bin/bash

OUT=./main

gcc ex2.c -lm -o $OUT
$OUT
rm $OUT
