#!/bin/bash

n=$1

rm -rf /tmp/ex1

gcc publisher.c -o pub
gcc subscriber.c -o sub

tmux start-server


tmux new-session -s Ex1Session -d "./pub $n"
for ((i=1; i<=n; i++))
do
    tmux split-window -t Ex1Session:0 "./sub $i"
done

tmux select-layout -t Ex1Session:0 main-vertical
tmux select-pane -t 0

tmux attach -t Ex1Session:0


rm {pub,sub}
