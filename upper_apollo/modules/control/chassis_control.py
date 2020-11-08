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
# from modules.control.proto import lin_control_cmd_pb2
from modules.guardian.proto import guardian_pb2

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
    rospy.init_node("chassis_control", anonymous=False)
    cmd_vel = rospy.Publisher('/cmd_vel', Twist, queue_size=5)

    # rospy.Subscriber('/apollo/lin_control', lin_control_cmd_pb2.LinControlCommand,
    #                  lin_control_callback)
    rospy.Subscriber('/apollo/guardian', guardian_pb2.GuardianCommand,
                     control_callback)

    move_cmd = Twist()

    # send navigation info to /apollo/navigation
    r = rospy.Rate(0.5)  # 0.5hz
    while not rospy.is_shutdown():       
        r.sleep()
