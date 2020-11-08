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
from nav_msgs.msg import Odometry
from std_msgs.msg import Int32MultiArray

#from modules.control.proto import lin_control_cmd_pb2 
from modules.canbus.proto import chassis_pb2

def odomCallback(data):
    chassis.speed_mps= 0.1#data.twist.twist.linear.y

def ultrasoniCallback(data):
    i = 0
    for i in range(9):
        chassis.surround.sonar_range[i] = data.data[i]
    
        #print(data.data[i])
        #print(chassis.surround.sonar_range00)
    

if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("lin_chassis", anonymous=False)

    chassis_pub = rospy.Publisher('/apollo/canbus/chassis', chassis_pb2.Chassis,queue_size=1)
    sub_odom = rospy.Subscriber('/odom', Odometry, odomCallback)
    sub_odom = rospy.Subscriber('/ultrasound', Int32MultiArray, ultrasoniCallback)
    pub_odom = rospy.Publisher('/odom', Odometry,queue_size=1)
    pub_ultrasonic = rospy.Publisher('/ultrasound', Int32MultiArray,queue_size=1)

    chassis = chassis_pb2.Chassis()
    
    odomdata = Odometry()

    ultrasonicdata = Int32MultiArray()
    pub_odom.publish(odomdata)
    pub_ultrasonic.publish(ultrasonicdata)

    chassis.engine_started = True
    chassis.engine_rpm = 1934.0

    chassis.odometer_m= 0.0
    chassis.fuel_range_m= 0
    chassis.throttle_percentage= 33.2448310852
    chassis.brake_percentage= 14.0077819824
    chassis.steering_percentage= 4.57446813583
    chassis.steering_torque_nm= -0.125
    chassis.parking_brake= False
    chassis.driving_mode= 1
    chassis.error_code= 0
    chassis.gear_location= 1 

    chassis.header.timestamp_sec = 1514497069.75
    chassis.header.module_name= "chassis"
    chassis.header.sequence_num= 20126
    chassis.signal.turn_signal= 0
    chassis.signal.horn= False

    chassis.chassis_gps.latitude= 37.4088293333
    chassis.chassis_gps.longitude= -122.010922667
    chassis.chassis_gps.gps_valid= True
    chassis.chassis_gps.year= 18
    chassis.chassis_gps.month= 8
    chassis.chassis_gps.day= 4
    chassis.chassis_gps.hours= 21
    chassis.chassis_gps.minutes= 37
    chassis.chassis_gps.seconds= 49
    chassis.chassis_gps.compass_direction= 45.0
    chassis.chassis_gps.pdop= 1.0
    chassis.chassis_gps.is_gps_fault= False
    chassis.chassis_gps.is_inferred= False
    chassis.chassis_gps.altitude= -42.5
    chassis.chassis_gps.heading= 28.07
    chassis.chassis_gps.hdop= 0.6
    chassis.chassis_gps.vdop= 0.6
    chassis.chassis_gps.quality= 2
    chassis.chassis_gps.num_satellites= 19
    chassis.chassis_gps.gps_speed= 5.81152


    chassis.surround.sonar_enabled = True
    chassis.surround.sonar_fault   = False
    

    i = 0
    for i in range(9):
        chassis.surround.sonar_range.append(0)
    
    r = rospy.Rate(20)  # 20hz
    while not rospy.is_shutdown():  
        chassis_pub.publish(chassis)
        r.sleep()
