import rospy
import sys
import json
import math
from std_msgs.msg import String
from sensor_msgs.msg import Imu
from modules.drivers.gnss.proto import imu_pb2
from modules.localization.proto import imu_pb2

#euler.X = atan2(2 * (q.y*q.z + q.w*q.x), q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
#euler.Y = asin(-2 * (q.x*q.z - q.w*q.y));
#euler.Z = atan2(2 * (q.x*q.y + q.w*q.z), q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z);
def callback(data):
    lin_Imu.header.timestamp_sec = rospy.get_time()
#    lin_Imu.measurement_time = 1.0 * data.header.stamp.secs + 1.0 * data.header.stamp.nsecs /1000000
#    lin_Imu.measurement_span = 0.00499999988824
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
   


if __name__ == '__main__':
    navi_files = sys.argv[1:]
    rospy.init_node("lin_imu", anonymous=False) #

    lin_imu_pub = rospy.Publisher('/apollo/sensor/gnss/corrected_imu', imu_pb2.CorrectedImu,queue_size=1)
    sub_imu = rospy.Subscriber('/imu', Imu, callback)
    
    pub = rospy.Publisher('/imu', Imu,queue_size=1)

    lin_Imu     = imu_pb2.CorrectedImu()

    lin_imudata = Imu()

    pub.publish(lin_imudata)

    r = rospy.Rate(100)  # 0.5hz
    while not rospy.is_shutdown():  
#        lin_Imu.header.timestamp_sec = rospy.get_time()

        #lin_Imu.imu.linear_velocity.x = 0.6
        #lin_Imu.imu.linear_velocity.y = 0.6
        #lin_Imu.imu.linear_velocity.z = 0.6


#        lin_Imu.imu.linear_acceleration.x = 0.1
#        lin_Imu.imu.linear_acceleration.y = 0.1
#        lin_Imu.imu.linear_acceleration.z = 0.1

#        lin_Imu.imu.angular_velocity.x = 0.1
#        lin_Imu.imu.angular_velocity.y = 0.1
#        lin_Imu.imu.angular_velocity.z = 0.1
#        lin_imu_pub.publish(lin_Imu)
        r.sleep()
