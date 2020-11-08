#!/bin/bash

echo "source /apollo/envset" >> /root/.bashrc
echo "source /apollo/ros_set" >> /root/.bashrc
echo "source /home/tmp/ros/setup.bash" >> /root/.bashrc

source /root/.bashrc
