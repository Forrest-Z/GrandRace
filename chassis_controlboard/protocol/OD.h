#ifndef __OD_H__
#define __OD_H__
#include <stdio.h>
#include <stdlib.h>
#include "info_callback.h"
#include "callback.h"
#include "buffer.h"
#include "info_core.h"
#include "protocol.h"
#include "alarm_com.h"
#include "Publish_outboard.h"

#pragma pack(1)
typedef struct {
    uint8_t board_type;
    uint8_t update_cmd;
    uint16_t num;
    uint8_t buffer[128];
} receive_buffer_;

typedef struct {
    uint8_t board_type;
    uint8_t update_cmd;
    uint16_t num;
    uint8_t buffer[128];
} down_send_buffer_;


extern uint8_t getPowerVer;
extern uint8_t getJoyVer;
extern uint8_t getUltVer;
extern uint8_t get_ledversin;
extern PUBLISH_FRAME_STRUCT MainBoard_OnlineUpdaye_Status_frame;
extern PUBLISH_FRAME_STRUCT Down_send_powerbuffer_frame;
extern PUBLISH_FRAME_STRUCT Down_send_sensorbuffer_frame;
extern PUBLISH_FRAME_STRUCT Down_send_ultbuffer_frame;
extern PUBLISH_FRAME_STRUCT Down_send_lightbuffer_frame;
extern PUBLISH_FRAME_STRUCT COM_DELAYPUB_frame;
extern receive_buffer_ receive_buffer;
extern down_send_buffer_ down_send_powerboard;
extern down_send_buffer_ down_send_sensorboard;
extern down_send_buffer_ down_send_ultboard;
extern down_send_buffer_ down_send_lightboard;
extern const Publish_struct publish[];
extern const Subscribe_struct subscribe[];

extern float speed;
extern float_struct Ultrasonic;

extern float_struct imu;
extern unsigned char testa_data[9];
extern Bms_Buffer Bms_Info;
extern char Power_version[30];
extern char Joy_version[30];
extern char ult_version[30];
extern char led_version[30];


extern Joystick_InfoTypeDef joyComBuff;
extern State_Switch_DownTypeDef State_Switch_Sub_Buffer;
extern Log_DownTypeDef Log_Down_Sub_Buffer;
extern Cmd_VelTypeDef Cmd_Vel_Sub_Buffer;
extern Control_COFTypeDef	Control_COF_Sub_Buffer;
extern Chassis_SpeedLimitTypeDef Chassis_speed_limit;
extern Tx2_PoweroffTypeDef Tx2_Poweroff_Ack_Buffer;
extern STATISTICS_CONTOTX2_MSG statistics_contotx2_msg;
extern STATISTICS_TX2toCON_msg statistics_tx2tocon_msg;

extern PUBLISH_FRAME_STRUCT PowerVersion_frame;
extern PUBLISH_FRAME_STRUCT JoyVersion_frame;
extern PUBLISH_FRAME_STRUCT UltVersion_frame;
extern PUBLISH_FRAME_STRUCT JoyReset_frame;
extern PUBLISH_FRAME_STRUCT LedVersion_frame;
extern PUBLISH_FRAME_STRUCT LedControl_frame;
extern PUBLISH_FRAME_STRUCT imu_frame;
extern PUBLISH_FRAME_STRUCT test_data_frame;
extern PUBLISH_FRAME_STRUCT Power_Control_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Battery_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Monitor_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Ultrasonic_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Log_Up_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT State_Switch_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Imu_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Odom_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Event_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Control_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Chassis_Speed_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Chassis_Mileage_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Tx2_Poweroff_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Ultrasonic_Up_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Ultrasonic_Sample_Send_Buffer_frame;
extern PUBLISH_FRAME_STRUCT JoysLogTp_Send_Buffer_frame;
extern PUBLISH_FRAME_STRUCT Alarm_Pub_Buffer_frame;
extern PUBLISH_FRAME_STRUCT SetUltCh_frame;
extern PUBLISH_FRAME_STRUCT GetUltCh_frame;

extern PUBLISH_FRAME_STRUCT statistics_contotx2_msg_frame;
extern PUBLISH_FRAME_STRUCT statistics_tx2tocon_msg_frame;
extern char Device_Recivce[16];
extern char Device_Send[16];


extern Battery_InfoTypeDef  Battery_Pub_Buffer;
extern Monitor_InfoTypeDef Monitor_Pub_Buffer;
extern Ultrasonic_InfoTypeDef Ultrasonic_Pub_Buffer;
extern Log_UpTypeDef Log_Up_Pub_Buffer;
extern State_Switch_UpTypeDef State_Switch_Pub_Buffer;
extern Imu_InfoTypeDef Imu_Pub_Buffer;
extern Odom_InfoTypeDef Odom_Pub_Buffer;
extern State_Event_UpTypeDef Event_Pub_Buffer;
extern Control_VelTypeDef Control_Pub_Buffer;
extern Power_HandleTypeDef Power_Control_Buffer;
extern Chassis_SpeedTypeDef Chassis_Speed_Buffer;
extern Chassis_MileageTypeDef Chassis_Mileage_Buffer;
extern Tx2_PoweroffTypeDef Tx2_Poweroff_Buffer;
extern Power_HandleTypeDef Power_Request_Buffer;
extern Control_InfoTypeDef Control_Info_Pub_Buffer;

extern Ultrasonic_CommTypeDef Ultrasonic_Up_Pub_Buffer;
extern Ultrasonic_SampleTypeDef Ultrasonic_Down_Sub_Buffer;
extern Ultrasonic_SampleTypeDef Ultrasonic_Sample_Send_Buffer;
extern Ultrasonic_SampleTypeDef Ultrasonic_Sample_Recv_Buffer;
extern char tx2_delay_flag;
extern char com_delay_pub;
//extern Log_TPTypeDef g_joysLogSubBufer;	//接收摇杆log tp 数据
//extern Log_TPTypeDef g_joysLogPubBufer;	//发送摇杆log tp 数据


int cal_Publish_size(void);
int cal_subscribe_size(void);


#endif
