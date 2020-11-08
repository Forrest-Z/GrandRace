#ifndef __CONTROLLER_h__
#define __CONTROLLER_h__
#include "control_config.h"

#pragma pack(1)

typedef struct
{
    float KP;
    float KI;
    float KD;

    float goal;
    float real;
    float lastTimeReal;
    float err[3];
    float integral;
    float integral_K_Max;
    float pid_out;
    unsigned char EN;
} PID_Typedef;

void IncrementalPidControler(PID_Typedef *pid,float diff_time);
void PositionalPidControler(PID_Typedef *pid,float diff_time);
void OpenLoopController(PID_Typedef *pid);
float	Linear_V_FrontFeed(float arg,MOTORCONTROL_Typedef *motor_arg);
void PID_Calc(PID_Typedef *pid);
#endif
