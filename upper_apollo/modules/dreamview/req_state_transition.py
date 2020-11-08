import rospy
import sys
from std_msgs.msg import UInt8

def callback(msg):
    
    pub.publish(msg)

if __name__ == '__main__':
    rospy.init_node("req_state_transition", anonymous=False)

    pub = rospy.Publisher('/ReqStateTransition', UInt8, queue_size=1)
    sub = rospy.Subscriber('/apollo/dreamview/ReqStateTransition', UInt8, callback)

    r = rospy.Rate(5)  # 5hz
    while not rospy.is_shutdown():  
        r.sleep()
