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
import math
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from std_msgs.msg import Int32MultiArray
from sensor_msgs.msg import Imu
from modules.drivers.gnss.proto import imu_pb2
from modules.localization.proto import imu_pb2
from modules.guardian.proto import guardian_pb2
from modules.localization.proto import localization_pb2
from modules.canbus.proto import chassis_pb2

def odomCallback(data):
    chassis.speed_mps= 0.1#data.twist.twist.linear.y

def ultrasoniCallback(data):
    i = 0
    for i in range(9):
        chassis.surround.sonar_range[i] = data.data[i]

def imu_callback(data):
    lin_Imu.header.timestamp_sec = rospy.get_time()
    q_x = data.orientation.x
    q_y = data.orientation.y
    q_z = data.orientation.z
    q_w = data.orientation.w

    lin_Imu.imu.linear_acceleration.x = -data.linear_acceleration.y
    lin_Imu.imu.linear_acceleration.y = -data.linear_acceleration.x
    lin_Imu.imu.linear_acceleration.z = data.linear_acceleration.z

    lin_Imu.imu.angular_velocity.x = -data.angular_velocity.y
    lin_Imu.imu.angular_velocity.y = -data.angular_velocity.x
    lin_Imu.imu.angular_velocity.z = data.angular_velocity.z

    lin_Imu.imu.euler_angles.x = math.atan2(2 * (q_y*q_z + q_w*q_x), q_w*q_w - q_x*q_x - q_y*q_y + q_z*q_z)
    lin_Imu.imu.euler_angles.y = math.asin(-2 * (q_x*q_z - q_w*q_y))
    lin_Imu.imu.euler_angles.z = math.atan2(2 * (q_x*q_y + q_w*q_z), q_w*q_w + q_x*q_x - q_y*q_y - q_z*q_z)

#    print "lin imu "
    lin_imu_pub.publish(lin_Imu)

#   lin_str = 'lin'

def control_callback(data):
   control_cmd_pb = guardian_pb2.GuardianCommand()
   control_cmd_pb.CopyFrom(data)

   if control_cmd_pb.control_command.linear_velocity > 0.05 and control_cmd_pb.control_command.linear_velocity < 0.1:
       control_cmd_pb.control_command.linear_velocity = 0.1

   elif control_cmd_pb.control_command.linear_velocity < -0.05 and control_cmd_pb.control_command.linear_velocity > -0.1:
       control_cmd_pb.control_command.linear_velocity = -0.1
   
   if control_cmd_pb.control_command.angular_velocity >0.05 and control_cmd_pb.control_command.angular_velocity < 0.11:
       control_cmd_pb.control_command.angular_velocity = 0.11

   elif control_cmd_pb.control_command.angular_velocity < -0.05 and control_cmd_pb.control_command.angular_velocity > -0.11:
       control_cmd_pb.control_command.angular_velocity = -0.11

   move_cmd.linear.x  = control_cmd_pb.control_command.linear_velocity
   move_cmd.angular.z = control_cmd_pb.control_command.angular_velocity

   cmd_vel.publish(move_cmd)

#/apollo/guardian
if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("chassis", anonymous=False)

    chassis_pub    = rospy.Publisher('/apollo/canbus/chassis', chassis_pb2.Chassis,queue_size=1)
    pub_odom       = rospy.Publisher('/odom', Odometry,queue_size=1)
    pub_ultrasonic = rospy.Publisher('/ultrasound', Int32MultiArray,queue_size=1)
    cmd_vel        = rospy.Publisher('/cmd_vel', Twist, queue_size=5)
    lin_imu_pub    = rospy.Publisher('/apollo/sensor/gnss/corrected_imu', imu_pb2.CorrectedImu,queue_size=1)
    imu_pub        = rospy.Publisher('/imu', Imu,queue_size=1)
    sub_odom       = rospy.Subscriber('/odom', Odometry, odomCallback)
    sub_ultrasound = rospy.Subscriber('/ultrasound', Int32MultiArray, ultrasoniCallback)
    sub_imu        = rospy.Subscriber('/imu', Imu, imu_callback)
    rospy.Subscriber('/apollo/guardian', guardian_pb2.GuardianCommand,
                     control_callback)

    
    chassis = chassis_pb2.Chassis()
    odomdata = Odometry()
    ultrasonicdata = Int32MultiArray()

    lin_Imu     = imu_pb2.CorrectedImu()
    lin_imudata = Imu()
    move_cmd = Twist()

    pub_odom.publish(odomdata)
    pub_ultrasonic.publish(ultrasonicdata)
    imu_pub.publish(lin_imudata)

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

    # send navigation info to /apollo/navigation
    r = rospy.Rate(20)  # 20hz
    while not rospy.is_shutdown(): 
        chassis_pub.publish(chassis)      
        r.sleep()
