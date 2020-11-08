#ifndef __INFO_CALLBACK_H__
#define __INFO_CALLBACK_H__

void InitTX2HeartSingalCallback(void);
void InitPowerBoardHeartSingalCallback(void);
void InitJoyBoardHeartSingalCallback(void);
void InitUltrasonicBoardHeartSingalCallback(void);
void InitlightstripBoardHeartSingalCallback(void);

void MainBoard_Online_Update_Callback(void);
void Down_send_powerbuffer_Callback(void);
void Down_send_joybuffer_Callback(void);
void Down_send_ultbuffer_Callback(void);
void Down_send_lightbuffer_Callback(void);
void InitUltrasonicSampleSendCallback(void);
void ResetUltrasonicCallback(void);
void InitJoysLogTpCallback(void); //初始化发送摇杆的故障注入消息
void InitGetPowerVersionCallback(void);
void InitGetJoyVersionCallback(void);
void InitGetUltVersionCallback(void);
void InitSetUltChannelCallback(void);
void InitGetUltChannelCallback(void);
void InitGetLedVersionCallback(void);
void InitLedControlCallback(void);
void InitCOMDELAYPUBCallback(void);
#endif
