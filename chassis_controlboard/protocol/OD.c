
#include "OD.h"
#include "Communication.h"
#include "Online_Downloader.h"
#include "Subscribe_Pc.h"

receive_buffer_ receive_buffer = {
    0,
    0,
    0,
};
down_send_buffer_ down_send_powerboard = {
    2,
    0,
    0,
};
down_send_buffer_ down_send_sensorboard = {
    3,
    0,
    0,
};
down_send_buffer_ down_send_ultboard = {
    4,
    0,
    0,
};
down_send_buffer_ down_send_lightboard = {
    5,
    0,
    0,
};
PUBLISH_FRAME_STRUCT MainBoard_OnlineUpdaye_Status_frame;
PUBLISH_FRAME_STRUCT Down_send_powerbuffer_frame;
PUBLISH_FRAME_STRUCT Down_send_sensorbuffer_frame;
PUBLISH_FRAME_STRUCT Down_send_ultbuffer_frame;
PUBLISH_FRAME_STRUCT Down_send_lightbuffer_frame;
PUBLISH_FRAME_STRUCT test_send_frame;
PUBLISH_FRAME_STRUCT PowerVersion_frame;
PUBLISH_FRAME_STRUCT JoyVersion_frame;
PUBLISH_FRAME_STRUCT JoyReset_frame;

PUBLISH_FRAME_STRUCT UltVersion_frame;
PUBLISH_FRAME_STRUCT SetUltCh_frame;
PUBLISH_FRAME_STRUCT GetUltCh_frame;
PUBLISH_FRAME_STRUCT LedVersion_frame;
PUBLISH_FRAME_STRUCT LedControl_frame;
PUBLISH_FRAME_STRUCT COM_DELAYPUB_frame;

float speed;
float_struct Ultrasonic;

float_struct imu;
unsigned char testa_data[9];

PUBLISH_FRAME_STRUCT imu_frame;
PUBLISH_FRAME_STRUCT test_data_frame;
PUBLISH_FRAME_STRUCT Power_Control_Buffer_frame;
PUBLISH_FRAME_STRUCT Battery_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Monitor_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Ultrasonic_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Log_Up_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT State_Switch_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Imu_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Odom_Pub_Buffer_frame;

PUBLISH_FRAME_STRUCT Event_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Control_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Chassis_Speed_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Chassis_Mileage_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Tx2_Poweroff_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Ultrasonic_Up_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Ultrasonic_Sample_Send_Buffer_frame;
PUBLISH_FRAME_STRUCT Ultrasonic_Reset_Buffer_frame;

PUBLISH_FRAME_STRUCT JoysLogTp_Send_Buffer_frame;
PUBLISH_FRAME_STRUCT statistics_contotx2_msg_frame;
PUBLISH_FRAME_STRUCT statistics_tx2tocon_msg_frame;
PUBLISH_FRAME_STRUCT Alarm_Pub_Buffer_frame;
PUBLISH_FRAME_STRUCT Control_Info_Buffer_frame;


char ok_buffer[] = "ok";
Bms_Buffer Bms_Info;
UltCh g_Ult_status;

uint8_t getPowerVer = 1;
uint8_t getJoyVer = 1;
uint8_t getUltVer = 1;
uint8_t get_ledversin = 1;


Power_HandleTypeDef Power_Control_Buffer 			 		= {0, 0};
Power_HandleTypeDef Power_Request_Buffer 			 		= {0, 0};
char Power_version[30] = {0};
char Joy_version[30] = {0};
char ult_version[30] = {0};
char led_version[30] = {0};

State_Switch_DownTypeDef State_Switch_Sub_Buffer  = {0};
Log_DownTypeDef Log_Down_Sub_Buffer								= {0, " "};
Cmd_VelTypeDef Cmd_Vel_Sub_Buffer								  = {0, 0, 0, 0, 0, 0};
Control_COFTypeDef	Control_COF_Sub_Buffer;


Battery_InfoTypeDef  Battery_Pub_Buffer  			 = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0};
Monitor_InfoTypeDef Monitor_Pub_Buffer   			 = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Ultrasonic_InfoTypeDef Ultrasonic_Pub_Buffer   = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Log_UpTypeDef Log_Up_Pub_Buffer  							 = {{0, 0, 0}, 0, " "};
State_Switch_UpTypeDef State_Switch_Pub_Buffer = {{0, 0, 0}, 0};
Imu_InfoTypeDef Imu_Pub_Buffer 								 = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
Odom_InfoTypeDef Odom_Pub_Buffer 							 = {{0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
State_Event_UpTypeDef Event_Pub_Buffer;
Control_VelTypeDef Control_Pub_Buffer;
Control_InfoTypeDef Control_Info_Pub_Buffer;
Joystick_InfoTypeDef joyComBuff;// = {0, 0, 0, 0, 0, 0, 0};
Chassis_SpeedTypeDef Chassis_Speed_Buffer = {0};
Chassis_MileageTypeDef Chassis_Mileage_Buffer = {0};
Tx2_PoweroffTypeDef Tx2_Poweroff_Buffer;
Tx2_PoweroffTypeDef Tx2_Poweroff_Ack_Buffer = {0};

Ultrasonic_CommTypeDef Ultrasonic_Up_Pub_Buffer = {{0, 0, 0}, 0, {0}};
Ultrasonic_SampleTypeDef Ultrasonic_Down_Sub_Buffer = {0, {0}};

Ultrasonic_SampleTypeDef Ultrasonic_Sample_Send_Buffer = {0, {0}};
Ultrasonic_SampleTypeDef Ultrasonic_Sample_Recv_Buffer = {0, {0}};

unsigned char Ultrasonic_Reset_Buffer = 0;


Chassis_SpeedLimitTypeDef Chassis_speed_limit;
STATISTICS_CONTOTX2_MSG statistics_contotx2_msg;
STATISTICS_TX2toCON_msg statistics_tx2tocon_msg = {0,1,0,0,0,0,0,0,0,0,1,0,0};


char Device_Recivce[16];
char Device_Send[16];
char com_delay_pub = 0;
char tx2_delay_flag = 0;



const Publish_struct publish[] = {
    {TX2_ID,BATTERY_INFO_ID,     		  &Battery_Pub_Buffer,  		 					&Battery_Pub_Buffer_frame,      				InitBatteryPubCallback},
    {TX2_ID,MONITOR_INFO_ID,     		  &Monitor_Pub_Buffer,  		 					&Monitor_Pub_Buffer_frame,      				InitMonitorPubCallback},
    {TX2_ID,ULTRASONIC_INFO_ID,       &Ultrasonic_Pub_Buffer,   					&Ultrasonic_Pub_Buffer_frame,   				InitUltrasonicPubCallback},
    {TX2_ID,LOG_UP_ID,     	          &Log_Up_Pub_Buffer,  		 					  &Log_Up_Pub_Buffer_frame,  	   					InitLogUpPubCallback},
    {TX2_ID,STATE_SWITCH_UP_ID,       &State_Switch_Pub_Buffer, 					&State_Switch_Pub_Buffer_frame, 				InitStateSwitchPubCallback},
    {TX2_ID,IMU_INFO_ID,              &Imu_Pub_Buffer, 				 					  &Imu_Pub_Buffer_frame, 				 					InitImuPubCallback},
    {TX2_ID,ODOM_INFO_ID,             &Odom_Pub_Buffer, 				 					&Odom_Pub_Buffer_frame, 				 				InitOdomPubCallback},
    {TX2_ID,EVENT_UP_ID,              &Event_Pub_Buffer, 			 						&Event_Pub_Buffer_frame, 		 						InitEventPubCallback},
    {TX2_ID,CONTROL_DEBUG_ID,         &Control_Pub_Buffer, 		 						&Control_Pub_Buffer_frame, 		 					InitControlPubCallback},
    {TX2_ID,CHASSIS_SPEED_ID,         &Chassis_Speed_Buffer,             	&Chassis_Speed_Pub_Buffer_frame,        InitChassisSpeedPubCallback},
    {TX2_ID,CHASSIS_MILEAGE_ID,       &Chassis_Mileage_Buffer,           	&Chassis_Mileage_Pub_Buffer_frame,      InitChassisMileagePubCallback},
    {TX2_ID,TX2_POWEROFF_ID,          &Tx2_Poweroff_Buffer,              	&Tx2_Poweroff_Pub_Buffer_frame,         InitTx2PoweroffPubCallback},
    {TX2_ID,ULTRASONIC_UP_ID,         &Ultrasonic_Up_Pub_Buffer,         	&Ultrasonic_Up_Pub_Buffer_frame,        InitUltrasonicUpPubCallback},
    {TX2_ID,CON_UP_SENDTOALE_ID,      &statistics_contotx2_msg,         	&statistics_contotx2_msg_frame,        	Initstatistics_contotx2_msgPubCallback},
    {TX2_ID,CON_UP_RECIVTOALE_ID,     &statistics_tx2tocon_msg,         	&statistics_tx2tocon_msg_frame,        	Initstatistics_tx2tocon_msgPubCallback},
		{TX2_ID,CONTROL_INFO_ID,          &Control_Info_Pub_Buffer,         	&statistics_tx2tocon_msg_frame,        	Initstatistics_tx2tocon_msgPubCallback},

    {TX2_ID,MainBoard_Online_Update_ID, &MainBoard_Online_Update_Status,&MainBoard_OnlineUpdaye_Status_frame,   &MainBoard_Online_Update_Callback},
		{TX2_ID,PUB_COM_DELAYPUB_ID, 				&com_delay_pub,                 &COM_DELAYPUB_frame,                    &InitCOMDELAYPUBCallback},
    {POW_ID,Power_Control_ID,     		 	&Power_Control_Buffer,  	      &Power_Control_Buffer_frame,            &InitPowerControlCallback},
    {POW_ID,power_down_send_ID,     		&down_send_powerboard,          &Down_send_powerbuffer_frame,   				&Down_send_powerbuffer_Callback},
    {POW_ID,PowerVer_ID,     					  &getPowerVer,             			&PowerVersion_frame,   									&InitGetPowerVersionCallback},
    {JOYS_ID,joy_down_send_ID,          &down_send_sensorboard,         &Down_send_sensorbuffer_frame,   				&Down_send_joybuffer_Callback},
    {JOYS_ID,JoyVer_ID,     					  &getJoyVer,             			  &JoyVersion_frame,   										&InitGetJoyVersionCallback},
    {JOYS_ID,PUB_JOY_RESET_ID,          &reset_joy,                     &JoyReset_frame,                        &InitGetJoyVersionCallback},
    {ULT_ID,PUB_ULT_SAMPLE_SEND_ID,  &Ultrasonic_Sample_Send_Buffer,&Ultrasonic_Sample_Send_Buffer_frame,    &InitUltrasonicSampleSendCallback},
    {ULT_ID,PUB_ULT_UPDATE_ID, &down_send_ultboard,   &Down_send_ultbuffer_frame,   &Down_send_ultbuffer_Callback},
    {ULT_ID,PUB_Ult_REBOOT_ID, &Ultrasonic_Reset_Buffer,   &Ultrasonic_Reset_Buffer_frame,   &ResetUltrasonicCallback},
    {TX2_ID,ALARM_PUB_ID,     &g_sAlarmPub,	         &Alarm_Pub_Buffer_frame,	     &InitAlarmPubCallback},
    {ULT_ID,PUB_UltVer_ID,    &getUltVer,            &UltVersion_frame,            &InitGetUltVersionCallback},
    {ULT_ID,PUB_Ult_SET_CHA_ID, &sSetUltCh,         &SetUltCh_frame,              &InitSetUltChannelCallback},
    {ULT_ID,PUB_Ult_GET_CHA_ID, &sGetUltCh,         &GetUltCh_frame,              &InitGetUltChannelCallback},
    {LED_ID,PUB_LED_VER_ID,   &get_ledversin,        &LedVersion_frame,   		     &InitGetLedVersionCallback},
    {LED_ID,Led_down_send_ID, &down_send_lightboard, &Down_send_lightbuffer_frame, &Down_send_lightbuffer_Callback},
    {LED_ID,PUB_LED_CTRL_ID,  &sLedCmd,              &LedControl_frame,   		     &InitLedControlCallback},
};

const Subscribe_struct subscribe[]= {
    {POW_ID,Bms_Info_ID,    		  &Bms_Info,          			 					NULL,  &bmsProtocolCallBack},
    {POW_ID,Power_Request_ID,     &Power_Request_Buffer,    					NULL,  &PowerRequestCallBack},
    {POW_ID,SUB_Power_ok_ID,    	&ok_buffer,         			 					NULL,  NULL},
    {POW_ID,PowerBoard_Online_ID, &PowerBoard_Online_Update_Status,   NULL,  &PowerBoard_Online_Update_Callback},
    {POW_ID,Power_version_ID,    	&Power_version,   									NULL,  &Power_VersionCallback},
    {TX2_ID,STATE_SWITCH_DOWN_ID, &State_Switch_Sub_Buffer, NULL,  &StateSwitchSubCallBack},
    {TX2_ID,LOG_DOWN_ID,          &Log_Down_Sub_Buffer,     NULL,  &LogDownSubCallBack},
    {TX2_ID,CMD_VEL_ID,           &Cmd_Vel_Sub_Buffer, 		  NULL,  &CmdVelbSubCallBack},
    {TX2_ID,SPEED_LIMIT,          &Chassis_speed_limit, 		NULL,  &ChassisSpeedLimitSubCallBack},
    {TX2_ID,TX2_POWEROFF_ACK_ID,  &Tx2_Poweroff_Ack_Buffer, NULL,  &Tx2PoweroffCallBack},
    {TX2_ID,CONTROL_PARA_ID,			&Control_COF_Sub_Buffer,	NULL,	 &Control_COF_CallBack},
    {TX2_ID,CON_SNEDFRAME_ID,  	  &Device_Send, 						NULL,  &CON_SENDFRAMECallback},
    {TX2_ID,CON_RECIVCEFRAME_ID,	&Device_Recivce,	 				NULL,	 &CON_RECIVCEFRAMECallback},
    {TX2_ID,MainBoard_Online_Update_ID,    &receive_buffer,                    NULL,  &Online_Update_Callback},

    {JOYS_ID,SensorBoard_Online_Update_ID, &SensorBoard_Online_Update_Status,  NULL,  &SensorBoard_Online_Update_Callback},
    {JOYS_ID,SUB_JOY_DATA_ID,    	&joyComBuff,              NULL,  &joystickProtocolCallBack},
    {JOYS_ID,SUB_JOY_VERSION_ID,  &Joy_version,   					NULL,  &Joy_VersionCallback},
    {JOYS_ID,SUB_JOY_RESET_ID,    &Joy_SubReset,            NULL,  &Joy_resetCallback},

    {TX2_ID,ULTRASONIC_DOWN_ID,   		 &Ultrasonic_Down_Sub_Buffer,      NULL,  &UltrasonicTX2CmdCallBack},
    {TX2_ID,COM_DELAY_ID,   		 			 &tx2_delay_flag,                  NULL,  &COM_DELAYFLAGCallback},
		{ULT_ID,SUB_ULT_SAMPLE_RECV_ID,    &Ultrasonic_Sample_Recv_Buffer,   NULL,  &UltrasonicSampleCmdCallBack},
    {ULT_ID,SUB_ULT_UPDATE_ID,         &UltBoard_Online_Update_Status,   NULL,  &UltBoard_Online_Update_Callback},
    {ULT_ID,SUB_UltVer_ID,             &ult_version,     								 NULL,  &ult_VersionCallback},
    {ULT_ID,SUB_Ult_GET_CHA_ID,        &g_Ult_status,                    NULL,  &ult_ChannelStatusCallback},
    {LED_ID,SUB_LED_VERSION_ID,    		 &led_version,   	 								 NULL,  &led_VersionCallback},
    {LED_ID,LedBoard_Online_Update_ID, &LightBoard_Online_Update_Status, NULL,  &LightBoard_Online_Update_Callback},
};

int cal_Publish_size()
{
    return 	sizeof(publish)/sizeof(Publish_struct);
}

int cal_subscribe_size()
{
    return sizeof(subscribe)/sizeof(Subscribe_struct);
}

