#!/usr/bin/env python

###############################################################################
# Copyright 2017 The Apollo Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###############################################################################

import rospy
import sys
import json
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from modules.control.proto import lin_control_cmd_pb2
from sensor_msgs.msg import Imu
from modules.localization.proto import localization_pb2
destination_x =0
destination_y =0
location_x =0
location_y =0
destination_error =0

def destination_callback(data):
    global destination_x 
    global destination_y 
    destination = localization_pb2.LocalizationEstimate()
    destination.CopyFrom(data)    
#    destination = Imu()
#    destination.CopyFrom(data)

#    destination_x = data.linear_acceleration.x
#    destination_y = data.linear_acceleration.y

    destination_x = destination.pose.position.x
    destination_y = destination.pose.position.y
    #print("destination_x",destination_x)
    #print("destination_y",destination_y)

    
def localization_callback(dalocalization_data):
    global location_x 
    global location_y 
    localization = localization_pb2.LocalizationEstimate()
    localization.CopyFrom(dalocalization_data)

    location_x = localization.pose.position.x
    location_y = localization.pose.position.y
    #print("location_x",location_x)
    #print("location_y",location_y)
    

def lin_control_callback(data):
    global destination_ 
    lin_control_cmd_pb = lin_control_cmd_pb2.LinControlCommand()
    lin_control_cmd_pb.CopyFrom(data)

    print("destination_x",destination_x)
    print("destination_y",destination_y)
    print("location_x",location_x)
    print("location_y",location_y)

    destination_error_x= abs(destination_x -location_x)
    destination_error_y= abs(destination_y - location_y)  
    print("destination_error_x",destination_error_x)
    print("destination_error_y",destination_error_y)
    if  abs(destination_error_x) < 2 and destination_error_y < 2:
        move_cmd.linear.x  = 0
        move_cmd.angular.z = 0
        cmd_vel.publish(move_cmd)        
        print("end")
    else:      
        move_cmd.linear.x  = lin_control_cmd_pb.linear_velocity
        move_cmd.angular.z = lin_control_cmd_pb.angular_velocity
	if lin_control_cmd_pb.linear_velocity >0.03 and lin_control_cmd_pb.linear_velocity <0.1:
   		move_cmd.linear.x  = 0.1
    	else:
        	move_cmd.linear.x  = lin_control_cmd_pb.linear_velocity
    	if lin_control_cmd_pb.angular_velocity >0.03 and lin_control_cmd_pb.angular_velocity <0.1:
        	move_cmd.angular.z = 0.1
    	else:
        	move_cmd.angular.z = lin_control_cmd_pb.angular_velocity
        cmd_vel.publish(move_cmd)


if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("lin_control", anonymous=False)
    cmd_vel = rospy.Publisher('/cmd_vel', Twist, queue_size=5)

    
    rospy.Subscriber('/apollo/vechile_destination', localization_pb2.LocalizationEstimate,
                     destination_callback)

    rospy.Subscriber('/apollo/localization/pose', localization_pb2.LocalizationEstimate,
                     localization_callback)

    rospy.Subscriber('/apollo/lin_control', lin_control_cmd_pb2.LinControlCommand,
                    lin_control_callback)                 

    move_cmd = Twist()


    # send navigation info to /apollo/navigation
    r = rospy.Rate(0.5)  # 0.5hz
    while not rospy.is_shutdown():       
        r.sleep()



