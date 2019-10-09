#!/bin/bash

# Note: Mininet must be run as root.  So invoke this shell script
# using sudo.

time=10
dir=output
name=projectTest
trace=DL_2_16Mbps

# project2 TCP
python run.py -tr $trace -t $time --name $name --dir $dir
python plot.py -tr $trace --name $name --dir $dir
