#!/bin/bash

gcc scheduler_rr.c -o sched
gcc worker.c -o worker

./sched data.txt

rm sched worker