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
from modules.map.relative_map.proto import navigation_pb2
from sensor_msgs.msg import Imu
from std_msgs.msg import Int64MultiArray
from modules.localization.proto import localization_pb2

if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("navigator_offline", anonymous=False)
    navigation_pub = rospy.Publisher(
        "/apollo/navigation", navigation_pb2.NavigationInfo, queue_size=1)
#    destination_pub = rospy.Publisher("/apollo/vechile_destination",Imu, queue_size=1)
    destination_pub = rospy.Publisher("/apollo/vechile_destination",localization_pb2.LocalizationEstimate, queue_size=1)
    destination_info = localization_pb2.LocalizationEstimate()

    # generate navigation info
    navigation_info = navigation_pb2.NavigationInfo()
    priority = 0
    for fdata in navi_files:
        print "processing " + fdata
        navigation_path = navigation_info.navigation_path.add()
        navigation_path.path_priority = priority
        priority += 1
        navigation_path.path.name = "navigation"

        f = open(fdata, 'r')
        cnt = 0
        for line in f:
            cnt += 1
            if cnt < 3:
                continue
            json_point = json.loads(line)
            point = navigation_path.path.path_point.add()
            point.x = json_point['x']
            point.y = json_point['y']
            point.s = json_point['s']
            point.theta = json_point['theta']
            point.kappa = json_point['kappa']
            point.dkappa = json_point['dkappa']
        destination_info.pose.position.x = point.x
        destination_info.pose.position.y = point.y
        #destination_info.linear_acceleration.y = point.y
        #destination_info[0] = point.x
        #destination_info[1] = point.y
        f.close()

    # send navigation info to /apollo/navigation
    r = rospy.Rate(10)  # 0.5hz
    while not rospy.is_shutdown():
        r.sleep()
        navigation_pub.publish(navigation_info)
        destination_pub.publish(destination_info)
        r.sleep()
       # break
