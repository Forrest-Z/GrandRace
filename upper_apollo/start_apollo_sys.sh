#!/bin/bash
function is_proc_ok()
{  
  proc=`ps -ef | grep $1 | grep modules | grep bazel-bin`  
  if [ $? -eq 0 ]  
  then    
    return 0
  else    
    return 1
  fi
}

function start_apollo_module()
{
  bash scripts/$1 start >/dev/null 2>&1
  if [ $? != 0 ]
  then
    echo "start $2 failed!"
    return 1
  else
    is_proc_ok $2
	if [ $? != 0 ]
	then
	  echo "start $2 failed!"
	  return 1
	else
      echo "start $2 success!"
      return 0
	fi
  fi
}

function start_rosserial()
{
  ps -ef | grep rosserial | grep -v grep | awk '{print $2}' | xargs kill -9
  ps -ef | grep roscore | grep -v grep | awk '{print $2}' | xargs kill -9
  ps -ef | grep rosmaster | grep -v grep | awk '{print $2}' | xargs kill -9
  nohup roscore >/dev/null 2>&1 &
  sleep 5 
  nohup rosrun rosserial_python serial_node.py _port:=/dev/tty_roserial _baud:=115200 >/dev/null 2>&1 &
  #roslaunch rosserial_python serial.launch
}

function start_gps()
{
  start_apollo_module gps.sh gnss
  return $?
}

function start_location()
{
  start_apollo_module localization.sh localization
  return $?
} 

function start_planning()
{
  start_apollo_module planning.sh planning
  return $?
} 

function start_relative_map()
{
  start_apollo_module relative_map.sh relative_map
  return $?
} 

function start_perception()
{
  start_apollo_module perception_offline_visualizer.sh perception
  return $?
} 

function start_prediction()
{
  start_apollo_module prediction.sh prediction
  return $?
} 

function start_control()
{
  start_apollo_module control.sh control
  return $?
} 

function start_guardian()
{
  start_apollo_module guardian.sh guardian
  return $?
} 


#########################################################
function start_goldenRidge_imu()
{
  #nohup python /apollo/modules/control/lin_imu.py
  ps -ef | grep lin_imu | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill lin_imu >/dev/null 2>&1
  fi
  
  nohup python /apollo/modules/control/lin_imu.py >/dev/null 2>&1 &
  ps -ef | grep lin_imu | grep -v grep
  if [ $? = 0 ]
  then
    echo "start goldenRidge_imu success!"
	return 0
  else
    echo "start goldenRidge_imu failed!"
	return 1
  fi
} 

function start_goldenRidge_control()
{
  ps -ef | grep lin_control | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill lin_control >/dev/null 2>&1
  fi
  
  nohup python /apollo/modules/control/lin_control.py >/dev/null 2>&1 &
  ps -ef | grep lin_control | grep -v grep
  if [ $? = 0 ]
  then
    echo "start goldenRidge_control success!"
	return 0
  else
    echo "start goldenRidge_control failed!"
	return 1
  fi
} 

function start_goldenRidge_chassis()
{ 
  ps -ef | grep lin_chassis | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill lin_chassis >/dev/null 2>&1
  fi
  
  nohup python /apollo/modules/control/lin_chassis.py >/dev/null 2>&1 &
  ps -ef | grep lin_chassis | grep -v grep
  if [ $? = 0 ]
  then
    echo "start goldenRidge_chassis success!"
	return 0
  else
    echo "start goldenRidge_chassis failed!"
	return 1
  fi
} 

function start_goldenRidge_chassis_new()
{ 
  ps -ef | grep chassis | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill chassis >/dev/null 2>&1
  fi
  
  nohup python /apollo/modules/control/chassis.py >/dev/null 2>&1 &
  ps -ef | grep chassis | grep -v grep
  if [ $? = 0 ]
  then
    echo "start chassis success!"
	return 0
  else
    echo "start chassis failed!"
	return 1
  fi
} 

function start_goldenRidge_safety()
{ 
  ps -ef | grep chassis_control | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill chassis_control >/dev/null 2>&1
  fi
  
  nohup python /apollo/modules/control/chassis_control.py >/dev/null 2>&1 &
  ps -ef | grep chassis_control | grep -v grep
  if [ $? = 0 ]
  then
    echo "start chassis_control success!"
	return 0
  else
    echo "start chassis_control failed!"
	return 1
  fi
} 

function start_camera()
{
  roslaunch usb_cam usb_cam-test.launch
  return 0
}


##########################################################
function main()
{
  source envset
  source ros_set
  source /home/tmp/ros/setup.bash 
  source catkin_ws/devel_isolated/setup.bash

  start_rosserial
  #nohub rosrun rosserial_python serial_node.py _port:=/dev/tty_rosserial _baud:=115200 >/dev/null 2>&1 &
  
  #start_camera
  start_gps
  start_goldenRidge_chassis_new
  ##start_goldenRidge_imu
  start_location
  ##start_goldenRidge_control
  ##start_goldenRidge_chassis
  #start_goldenRidge_safety
  start_relative_map
  #start_perception
  #start_prediction
  start_planning
  start_control
  #start_guardian
}

main



