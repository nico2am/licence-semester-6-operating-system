#!/bin/bash

for ((I=0; I<10; I++)); do
~/Documents/Systeme/TP1/job.sh &
done
wait
exit 0

