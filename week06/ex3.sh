#!/bin/bash

gcc scheduler_sjf.c -o sched
gcc worker.c -o worker

./sched data.txt

rm sched worker