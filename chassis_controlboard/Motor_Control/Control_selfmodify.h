#ifndef _CONTROL_SELFMODIFY_H
#define _CONTROL_SELFMODIFY_H
#include "Motor_Control.h"
#include "controller.h"

#pragma pack(4)
typedef struct
{
    float KP;
    float KI;
    float KD;
} Selfmod_PID_Typedef;

void Init_FuzzyPID(void);
void FuzzysetPIDPara(ControlData_Typedef *controlDataInst);
void Update_Para(ControlData_Typedef *controlDataInst);
#endif
