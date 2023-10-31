#!/bin/bash

gcc ex1.c -o ex1
./ex1 &
sleep 0.5
PID=$(cat /tmp/ex1.pid)

# `cat /proc/pid/maps` outputs the mapping info
# then `grep /dev/zero`` searches for the line containing `/dev/zero`
# then `awk {print $1}` prints the first word in this line
# this first word will be address range in format {start}-{end}
# next, I use a regular expression to take everything up to the - sign
# from the string "{start}-{end}", which is {start}
# next i look at the memory using xxd
# since I used mmap, os gived an empty page
# and the password is located at the very beginning of this page
# therefore, the password starts at START an
# goes 13 characters (5 for pass: and 8 for password)
# and in the end, I just take everything after the `pass:` using regex again

ADDRESSRANGE=$(cat /proc/$PID/maps | grep /dev/zero | awk "{print \$1}")
START=0x$(expr $ADDRESSRANGE : "\\(.*\\)-.*")
MEMORY=$(sudo xxd -s $START -l 13 /proc/$PID/mem)
PASSWORD=$(expr "$MEMORY" : ".*pass:\\(.*\\)")
echo ">> password is $PASSWORD"
echo ">> address in memory is $START"

# or using malloc:
# ADDRESSRANGE=$(cat /proc/$PID/maps | grep heap | awk "{print \$1}")
# START=0x$(expr $ADDRESSRANGE : "\\(.*\\)-.*")
# END=0x$(expr $ADDRESSRANGE : ".*-\\(.*\\)")
# MEMORY=$(sudo xxd -s $START -l $(($END - $START)) /proc/$PID/mem | cut -d' ' -f11- | tr -d '\n' | grep -oP "pass:.{8}")
# PASSWORD=$(expr "$MEMORY" : ".*pass:\\(.*\\)")
# echo ">> password is $PASSWORD"

kill -2 $PID # send sigint

rm ./ex1