#include "info_OD.h"

unsigned char TX2_heart_singal[4];
unsigned char power_board_heart_singal[4];
unsigned char sensor_board_heart_singal[4];
unsigned char ultrasonic_board_heart_singal[4];
unsigned char lightstrip_board_heart_singal[4];
unsigned char TX2_time[14];

PUBLISH_FRAME_STRUCT TX2_heartSignal_frame;
PUBLISH_FRAME_STRUCT power_board_heartSignal_frame;
PUBLISH_FRAME_STRUCT sensor_board_heartSignal_frame;
PUBLISH_FRAME_STRUCT ultrasonic_board_heartSignal_frame;
PUBLISH_FRAME_STRUCT lightstrip_board_heartSignal_frame;
PUBLISH_FRAME_STRUCT TX2_time_frame;

HEART_SINGAL_TIME_STRUCT TX2_heart_singal_time;
HEART_SINGAL_TIME_STRUCT power_board_heart_singal_time;
HEART_SINGAL_TIME_STRUCT joy_board_heart_singal_time;
HEART_SINGAL_TIME_STRUCT ultrasonic_board_heart_singal_time;
HEART_SINGAL_TIME_STRUCT lightstrip_board_heart_singal_time;
HEART_SINGAL_TIME_STRUCT TX2_time_time;

BUFFER_DATA *lin_buffer_data[] = {&powerBoard_buffer,&sensorBoard_buffer,&TX2Board_buffer,&ultrasonicBoard_buffer,&LightstripBoard_buffer};
const Publish_struct heartSingal[]= {
    {TX2_ID,  0,    &TX2_heart_singal,              &TX2_heartSignal_frame,          		  &InitTX2HeartSingalCallback},
    {POW_ID,  0,    &power_board_heart_singal,      &power_board_heartSignal_frame,   		&InitPowerBoardHeartSingalCallback},
    {JOYS_ID, 0,    &sensor_board_heart_singal,     &sensor_board_heartSignal_frame,  	  &InitJoyBoardHeartSingalCallback},
    {ULT_ID,  0,    &ultrasonic_board_heart_singal, &ultrasonic_board_heartSignal_frame,  &InitUltrasonicBoardHeartSingalCallback},
    {LED_ID,  0,    &lightstrip_board_heart_singal, &lightstrip_board_heartSignal_frame,  &InitlightstripBoardHeartSingalCallback},
    {TX2_ID,  1,    &TX2_time,                  		&TX2_time_frame,                  		 NULL},
};

const HEART_SINGAL_STRUCT heart_singal_struct[]= {
    {TX2_ID,  0,  &TX2_heart_singal_time,            		 &transmitTX2Data},
    {POW_ID,  0,  &power_board_heart_singal_time,    		 &transmitPowerBoardData},
    {JOYS_ID, 0,  &joy_board_heart_singal_time,   		 &transmitJoyBoardData},
    {ULT_ID,  0, 	&ultrasonic_board_heart_singal_time,   &transmitUltrasonicBoardData},
    {LED_ID,  0, 	&lightstrip_board_heart_singal_time,   &transmitLightstripBoardData},
    {TX2_ID,  1,  &TX2_time_time,                    		  NULL},
};

int cal_lin_buffer_size()
{
    return 	sizeof(lin_buffer_data)/sizeof(BUFFER_DATA*);
}

int cal_heartSingal_size()
{
    return sizeof(heart_singal_struct)/sizeof(HEART_SINGAL_STRUCT);
}
