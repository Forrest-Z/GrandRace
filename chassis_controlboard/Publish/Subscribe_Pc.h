#ifndef __SUBSCRIBE_PC_H
#define __SUBSCRIBE_PC_H
#include <stdint.h>

void StateSwitchSubCallBack(void);
void LogDownSubCallBack(void);
void UpdateRemoteControlData(float *data);
void ResetRemoteControlData(void);
void CmdVelbSubCallBack(void);
void ChassisSpeedLimitSubCallBack(void);
void Control_COF_CallBack(void);
void Tx2PoweroffCallBack(void);

uint8_t Tx2PoweroffGetAck(void);
void Tx2PoweroffCleanAck(void);
void UltrasonicTX2CmdCallBack(void);
void UltrasonicSampleCmdCallBack(void);
void CON_SENDFRAMECallback(void);
void CON_RECIVCEFRAMECallback(void);
void COM_DELAYFLAGCallback(void);

extern float speed_limit[2][2];
void GetSpeedLimitPara(float *pdata);
unsigned short int getUltrasonicModuleState(void);
uint8_t *getUltrasonicVersion(void);

#endif

