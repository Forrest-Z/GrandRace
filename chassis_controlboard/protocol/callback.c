#include "callback.h"
#include "OD.h"
#include <string.h>
#include "Publish_powerboard.h"
#include "Publish_Pc.h"

void InitPowerControlCallback(void)
{
    Power_Control_Buffer_frame.sFrameData.ack = 0;
    Power_Control_Buffer_frame.sAck.resend_set_num = 10;
    Power_Control_Buffer_frame.sAck.over_time = 2; //ms
}

void InitBatteryPubCallback(void)
{
    Battery_Pub_Buffer_frame.sFrameData.ack = 0;
    Battery_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Battery_Pub_Buffer_frame.sAck.over_time = 5; //ms
}


void InitMonitorPubCallback(void)
{
    Monitor_Pub_Buffer_frame.sFrameData.ack = 0;
    Monitor_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Monitor_Pub_Buffer_frame.sAck.over_time = 5; //ms
}

void InitUltrasonicPubCallback(void)
{
    Ultrasonic_Pub_Buffer_frame.sFrameData.ack = 0;
    Ultrasonic_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Ultrasonic_Pub_Buffer_frame.sAck.over_time = 5; //ms
}

void InitLogUpPubCallback(void)
{
    Log_Up_Pub_Buffer_frame.sFrameData.ack = 0;
    Log_Up_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Log_Up_Pub_Buffer_frame.sAck.over_time = 20; //ms
}


void InitStateSwitchPubCallback(void)
{
    State_Switch_Pub_Buffer_frame.sFrameData.ack = 0;
    State_Switch_Pub_Buffer_frame.sAck.resend_set_num = 10;
    State_Switch_Pub_Buffer_frame.sAck.over_time = 20; //ms
}

void InitImuPubCallback(void)
{
    Imu_Pub_Buffer_frame.sFrameData.ack = 0;
    Imu_Pub_Buffer_frame.sAck.resend_set_num = 100;
    Imu_Pub_Buffer_frame.sAck.over_time = 5; //ms
}

void InitOdomPubCallback(void)
{
    Odom_Pub_Buffer_frame.sFrameData.ack = 0;
    Odom_Pub_Buffer_frame.sAck.resend_set_num = 100;
    Odom_Pub_Buffer_frame.sAck.over_time = 5; //ms
}

void InitEventPubCallback(void)
{
    Event_Pub_Buffer_frame.sFrameData.ack = 1;
    Event_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Event_Pub_Buffer_frame.sAck.over_time = 20; //ms
}

void InitControlPubCallback(void)
{
    Control_Pub_Buffer_frame.sFrameData.ack = 0;
}

void InitChassisSpeedPubCallback(void)
{
    Chassis_Speed_Pub_Buffer_frame.sFrameData.ack = 0;
}

void InitChassisMileagePubCallback(void)
{
    Chassis_Mileage_Pub_Buffer_frame.sFrameData.ack = 0;
}

void InitTx2PoweroffPubCallback(void)
{
    Tx2_Poweroff_Pub_Buffer_frame.sFrameData.ack = 0;
}


void InitUltrasonicUpPubCallback(void)
{
    Ultrasonic_Up_Pub_Buffer_frame.sFrameData.ack = 0;
    Ultrasonic_Up_Pub_Buffer_frame.sAck.resend_set_num = 10;
    Ultrasonic_Up_Pub_Buffer_frame.sAck.over_time = 5; //ms
}
void Initstatistics_contotx2_msgPubCallback(void)
{
    statistics_contotx2_msg_frame.sFrameData.ack = 0;
    statistics_contotx2_msg_frame.sAck.resend_set_num = 10;
    statistics_contotx2_msg_frame.sAck.over_time = 5; //ms
}
void Initstatistics_tx2tocon_msgPubCallback(void)
{
    statistics_tx2tocon_msg_frame.sFrameData.ack = 0;
    statistics_tx2tocon_msg_frame.sAck.resend_set_num = 10;
    statistics_tx2tocon_msg_frame.sAck.over_time = 5; //ms
}

void InitAlarmPubCallback(void)
{
    Alarm_Pub_Buffer_frame.sFrameData.ack = 0;
}





