#!/bin/bash

OUT=./main

gcc ex4.c -lreadline -o $OUT
$OUT
rm $OUT
