#!/bin/bash

# sudo su
# echo 1 > /sys/devices/system/cpu/cpu1/online
# echo 1 > /sys/devices/system/cpu/cpu2/online

nvpmodel -p --verbose
nvpmodel -m 0 

cd   /home/nvidia 
sudo  ./jetson_clocks.sh
