#!/bin/bash

gcc scheduler.c -o sched
gcc worker.c -o worker

./sched data.txt

rm sched worker