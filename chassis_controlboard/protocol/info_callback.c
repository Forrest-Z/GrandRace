#include "info_callback.h"
#include "OD.h"
#include "info_OD.h"


void InitTX2HeartSingalCallback()
{

    TX2_heart_singal_time.offsetTime = 1000/TX2_HEARTSINGAL_HZ; //ms
    TX2_heartSignal_frame.sFrameData.ack = 1;
    TX2_heartSignal_frame.sAck.resend_set_num = 15;
    TX2_heartSignal_frame.sAck.over_time = 100; //ms
}

void InitPowerBoardHeartSingalCallback()
{
    power_board_heart_singal_time.offsetTime = 1000/POWER_BOARD_HEARTSINGAL_HZ; //ms
    power_board_heartSignal_frame.sFrameData.ack = 1;
    power_board_heartSignal_frame.sAck.resend_set_num = 15;
    power_board_heartSignal_frame.sAck.over_time = 100; //ms
}

void InitJoyBoardHeartSingalCallback()
{
    joy_board_heart_singal_time.offsetTime = 1000/SENSOR_BOARD_HEARTSINGAL_HZ; //ms
    sensor_board_heartSignal_frame.sFrameData.ack = 1;
    sensor_board_heartSignal_frame.sAck.resend_set_num = 15;
    sensor_board_heartSignal_frame.sAck.over_time = 100; //ms
}

void InitUltrasonicBoardHeartSingalCallback()
{
    ultrasonic_board_heart_singal_time.offsetTime = 1000/ULTRASONIC_BOARD_HEARTSINGAL_HZ; //ms
    ultrasonic_board_heartSignal_frame.sFrameData.ack = 1;
    ultrasonic_board_heartSignal_frame.sAck.resend_set_num = 10;
    ultrasonic_board_heartSignal_frame.sAck.over_time = 100; //ms
}
void InitlightstripBoardHeartSingalCallback(void)
{
    lightstrip_board_heart_singal_time.offsetTime = 1000/LIGHTSTRIP_BOARD_HEARTSINGAL_HZ; //ms
    lightstrip_board_heartSignal_frame.sFrameData.ack = 1;
    lightstrip_board_heartSignal_frame.sAck.resend_set_num = 10;
    lightstrip_board_heartSignal_frame.sAck.over_time = 100; //ms
}

void MainBoard_Online_Update_Callback(void)
{
    MainBoard_OnlineUpdaye_Status_frame.sFrameData.ack = 0;
    MainBoard_OnlineUpdaye_Status_frame.sAck.resend_set_num = 10;
    MainBoard_OnlineUpdaye_Status_frame.sAck.over_time = 100;
}
void Down_send_powerbuffer_Callback(void)
{
    Down_send_powerbuffer_frame.sFrameData.ack = 0;
    Down_send_powerbuffer_frame.sAck.resend_set_num = 10;
    Down_send_powerbuffer_frame.sAck.over_time = 100;
}
void Down_send_joybuffer_Callback(void)
{
    Down_send_sensorbuffer_frame.sFrameData.ack = 0;
    Down_send_sensorbuffer_frame.sAck.resend_set_num = 10;
    Down_send_sensorbuffer_frame.sAck.over_time = 100;
}
void Down_send_ultbuffer_Callback(void)
{
    Down_send_ultbuffer_frame.sFrameData.ack = 0;
    Down_send_ultbuffer_frame.sAck.resend_set_num = 10;
    Down_send_ultbuffer_frame.sAck.over_time = 100;
}
void Down_send_lightbuffer_Callback(void)
{
    Down_send_lightbuffer_frame.sFrameData.ack = 0;
    Down_send_lightbuffer_frame.sAck.resend_set_num = 10;
    Down_send_lightbuffer_frame.sAck.over_time = 100;
}
void  InitUltrasonicSampleSendCallback(void)
{
    Ultrasonic_Sample_Send_Buffer_frame.sFrameData.ack = 0;
    Ultrasonic_Sample_Send_Buffer_frame.sAck.resend_set_num = 10;
    Ultrasonic_Sample_Send_Buffer_frame.sAck.over_time = 100;
}

void ResetUltrasonicCallback(void)
{
    Down_send_ultbuffer_frame.sFrameData.ack = 0;
    Down_send_ultbuffer_frame.sAck.resend_set_num = 10;
    Down_send_ultbuffer_frame.sAck.over_time = 100;
}


void  InitJoysLogTpCallback(void)
{
    JoysLogTp_Send_Buffer_frame.sFrameData.ack = 0;
    JoysLogTp_Send_Buffer_frame.sAck.resend_set_num = 3;
    JoysLogTp_Send_Buffer_frame.sAck.over_time = 100;
}
void InitGetPowerVersionCallback(void)
{
    PowerVersion_frame.sFrameData.ack = 0;
    PowerVersion_frame.sAck.resend_set_num = 3;
    PowerVersion_frame.sAck.over_time = 100;
}
void InitGetJoyVersionCallback(void)
{
    JoyVersion_frame.sFrameData.ack = 0;
    JoyVersion_frame.sAck.resend_set_num = 3;
    JoyVersion_frame.sAck.over_time = 100;
}
void InitGetJoyResetCallback(void)
{
    JoyReset_frame.sFrameData.ack = 0;
    JoyReset_frame.sAck.resend_set_num = 3;
    JoyReset_frame.sAck.over_time = 100;
}

void InitGetUltVersionCallback(void)
{
    UltVersion_frame.sFrameData.ack = 0;
    UltVersion_frame.sAck.resend_set_num = 3;
    UltVersion_frame.sAck.over_time = 100;
}
void InitSetUltChannelCallback(void)
{
    SetUltCh_frame.sFrameData.ack = 0;
    SetUltCh_frame.sAck.resend_set_num = 15;
    SetUltCh_frame.sAck.over_time = 100;
}
void InitGetUltChannelCallback(void)
{
    GetUltCh_frame.sFrameData.ack = 0;
    GetUltCh_frame.sAck.resend_set_num = 15;
    GetUltCh_frame.sAck.over_time = 100;
}

void InitGetLedVersionCallback(void)
{
    LedVersion_frame.sFrameData.ack = 0;
    LedVersion_frame.sAck.resend_set_num = 3;
    LedVersion_frame.sAck.over_time = 100;
}
void InitLedControlCallback(void)
{
    LedControl_frame.sFrameData.ack = 0;
    LedControl_frame.sAck.resend_set_num = 3;
    LedControl_frame.sAck.over_time = 100;
}
void InitCOMDELAYPUBCallback(void)
{
	  COM_DELAYPUB_frame.sFrameData.ack = 0;
    COM_DELAYPUB_frame.sAck.resend_set_num = 3;
    COM_DELAYPUB_frame.sAck.over_time = 100;
}
