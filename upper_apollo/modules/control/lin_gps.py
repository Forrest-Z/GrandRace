import rospy
import sys
import json
import math
from std_msgs.msg import String
from nav_msgs.msg import Odometry
from sensor_msgs.msg import Imu
from modules.localization.proto import gps_pb2


def imucallback(data):

    lin_Gps.localization.orientation.qx = data.orientation.x
    lin_Gps.localization.orientation.qy = data.orientation.y
    lin_Gps.localization.orientation.qz = data.orientation.z
    lin_Gps.localization.orientation.qw = data.orientation.w


#var ;
#var ;
#var ;

def callback(data):

    lin_Gps.header.timestamp_sec = rospy.get_time()
    radius = 6371000.0
    lt = data.localization.position.y
    lg = data.localization.position.x
    if lt != 0 or lg != 0:
        y = radius * math.sin(lt)
        temp = radius * math.cos(lt)
        x = temp * math.sin(lg)

        lin_Gps.localization.position.x = data.localization.position.x + 587544 - 410927.565587
        lin_Gps.localization.position.y = data.localization.position.y + 4140712 - 3363324.38229
        lin_Gps.localization.position.z = data.localization.position.z - 29.836172784 - 464.3306

        lin_Gps.localization.linear_velocity.x = data.localization.linear_velocity.x
        lin_Gps.localization.linear_velocity.y = data.localization.linear_velocity.y
        lin_Gps.localization.linear_velocity.z = data.localization.linear_velocity.z

    #print "lin imu "
        lin_gps_pub.publish(lin_Gps)

#   lin_str = 'lin'
   


if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("lin_gps", anonymous=False) #

    lin_gps_pub = rospy.Publisher('/apollo/sensor/gnss/odometry', gps_pb2.Gps,queue_size=1)
    rospy.Subscriber('/apollo/sensor/gnss/lin_odometry', gps_pb2.Gps,callback)
    
    rospy.Subscriber('/imu', Imu, imucallback)
    
    pub = rospy.Publisher('/imu', Imu,queue_size=1)

    lin_imudata = Imu()

    lin_Gps = gps_pb2.Gps()

    pub.publish(lin_imudata)

    r = rospy.Rate(1)  # 0.5hz

    while not rospy.is_shutdown():  

        r.sleep()