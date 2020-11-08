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

function stop_apollo_module()
{
  bash scripts/$1 stop >/dev/null 2>&1
  if [ $? != 0 ]
  then
    echo "stop $2 failed!"
    return 1
  else
    is_proc_ok $2
	if [ $? != 0 ]
	then
	  echo "stop $2 success!"
	  return 1
	else
      echo "stop $2 failed!"
      return 0
	fi
  fi
}

function stop_rosserial()
{
  ps -ef | grep rosserial | grep -v grep | awk '{print $2}' | xargs kill -9
  ps -ef | grep roscore | grep -v grep | awk '{print $2}' | xargs kill -9
  ps -ef | grep rosmaster | grep -v grep | awk '{print $2}' | xargs kill -9
  #rosnode kill rosserial_python
}

function stop_gps()
{
  stop_apollo_module gps.sh gnss
  return $?
}

function stop_location()
{
  stop_apollo_module localization.sh localization
  return $?
} 

function stop_planning()
{
  stop_apollo_module planning.sh planning
  return $?
} 

function stop_relative_map()
{
  stop_apollo_module relative_map.sh relative_map
  return $?
} 

function stop_perception()
{
  stop_apollo_module perception_offline_visualizer.sh perception
  return $?
} 

function stop_prediction()
{
  stop_apollo_module prediction.sh prediction
  return $?
} 

function stop_control()
{
  stop_apollo_module control.sh control
  return $?
} 

function stop_guardian()
{
  stop_apollo_module guardian.sh guardian
  return $?
} 


#########################################################
function stop_goldenRidge_imu()
{
  ps -ef | grep lin_imu | grep -v grep
  if [ $? -ne 0 ]
  then
    rosnode kill lin_imu >/dev/null 2>&1
  fi
  
  sleep 2
  ps -ef | grep lin_imu | grep -v grep
  if [ $? = 0 ]
  then
    echo "stop goldenRidge_imu success!"
	return 0
  else
    echo "stop goldenRidge_imu failed!"
	return 1
  fi
} 

function stop_goldenRidge_control()
{
  ps -ef | grep lin_control | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill lin_control >/dev/null 2>&1
  fi
  
  sleep 2
  ps -ef | grep lin_control | grep -v grep
  if [ $? -ne 0 ]
  then
    echo "stop goldenRidge_control success!"
	return 0
  else
    echo "stop goldenRidge_control failed!"
	return 1
  fi
} 

function stop_goldenRidge_chassis()
{ 
  ps -ef | grep lin_chassis | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill lin_chassis >/dev/null 2>&1
  fi
  
  sleep 2
  ps -ef | grep lin_chassis | grep -v grep
  if [ $? -ne 0 ]
  then
    echo "stop goldenRidge_chassis success!"
	return 0
  else
    echo "stop goldenRidge_chassis failed!"
	return 1
  fi
} 

function stop_goldenRidge_chassis_new()
{ 
  ps -ef | grep chassis | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill chassis >/dev/null 2>&1
  fi
  
  sleep 2
  ps -ef | grep chassis | grep -v grep
  if [ $? -ne 0 ]
  then
    echo "stop chassis success!"
	return 0
  else
    echo "stop chassis failed!"
	return 1
  fi
} 

function stop_goldenRidge_safety()
{ 
  ps -ef | grep chassis_control | grep -v grep
  if [ $? = 0 ]
  then
    rosnode kill chassis_control >/dev/null 2>&1
  fi
  
  sleep 2
  ps -ef | grep chassis_control | grep -v grep
  if [ $? -ne 0 ]
  then
    echo "stop chassis_control success!"
	return 0
  else
    echo "stop chassis_control failed!"
	return 1
  fi
} 

function stop_camera()
{
  rosnode kill usb_cam
  return 0
}


##########################################################
function main()
{
  source envset
  source ros_set
  source /home/tmp/ros/setup.bash 
  source catkin_ws/devel_isolated/setup.bash
  
  stop_goldenRidge_chassis_new
  #stop_camera
  stop_gps
  ##stop_goldenRidge_imu
  stop_location
  ##stop_goldenRidge_control
  ##stop_goldenRidge_chassis
  #stop_goldenRidge_safety
  stop_relative_map
  #stop_perception
  #stopt_prediction
  stop_planning
  stop_control
  #stop_guardian
  stop_rosserial
}

main



