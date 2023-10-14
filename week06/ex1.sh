#!/bin/bash

AGENT=./agent
CONTR=./controller

gcc agent.c -o $AGENT
gcc controller.c -o $CONTR

sudo $AGENT & 

sudo $CONTR

rm $AGENT
rm $CONTR