#ifndef __PUBLISH_PC_H
#define __PUBLISH_PC_H

#define IMU_PUBLISH_FREQ           			30
#define ODOM_PUBLISH_FREQ          			30
#define BATTERY_PUBLISH_FREQ       			10
#define ULTRASONIC_PUBLISH_FREQ        	50//30
#define HEARTBEAT_PUBLISH_FREQ        	10
#define MONITOR_PUBLISH_FREQ 						10
#define CONTROL_DEBUG_PUBLISH_FREQ 			20
#define CHASSIS_SPEED_PUBLISH_FREQ 			2
#define CHASSIS_ALARM_CHANGE_FREQ 			100
#define CONTROL_INFO_PUBLISH_FREQ				20
#define ULTRASONIC_CHECK_FREQ        		1

#define No_SAVE_FRAME 0
#define SAVE_FRAME    1

//extern unsigned int  imu_test_time;
//void Send_StateSwitch(void);
//void Send_ChassisEvent(void);
void LogUpPubHandle(char* info, unsigned char len);
void Tx2PoweroffRequestPubHandle(void);

void infoPublishHanlder(void);

void UltrasonicSampleCmdPubHandle(void);
void con_to_tx2_framepub(unsigned char save_flag);
void tx2_to_con_framepub(unsigned char save_flag);
#endif

