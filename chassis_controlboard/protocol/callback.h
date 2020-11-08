#ifndef __CALLBACK_H__
#define __CALLBACK_H__
#include "Bms_Info.h"
#include "protocol.h"


#pragma pack(1)

typedef struct
{
    int length;
    float data[9];
} float_struct;

typedef struct {
    unsigned char ID_Code;
    unsigned char Function_Code;
} Power_HandleTypeDef;

void InitPowerControlCallback(void);
void InitBatteryPubCallback(void);
void InitMonitorPubCallback(void);
void InitUltrasonicPubCallback(void);
void InitLogUpPubCallback(void);
void InitStateSwitchPubCallback(void);
void InitImuPubCallback(void);
void InitOdomPubCallback(void);
void InitEventPubCallback(void);
void InitControlPubCallback(void);
void InitChassisSpeedPubCallback(void);
void InitChassisMileagePubCallback(void);
void InitTx2PoweroffPubCallback(void);
void InitUltrasonicUpPubCallback(void);
void Initstatistics_contotx2_msgPubCallback(void);
void Initstatistics_tx2tocon_msgPubCallback(void);
void InitAlarmPubCallback(void);

void bmsProtocolCallBack(void);
void joystickProtocolCallBack(void);
void PowerRequestCallBack(void);
void Power_VersionCallback(void);
void Joy_VersionCallback(void);
void ult_VersionCallback(void);
void led_VersionCallback(void);
void ult_ChannelStatusCallback(void);
void StateSwitchSubCallBack(void);
void LogDownSubCallBack(void);
void CmdVelbSubCallBack(void);

void UpdateRemoteControlData(float *data);
void ResetRemoteControlData(void);

void joys_log_tp_SubcallBack(void);


#endif

