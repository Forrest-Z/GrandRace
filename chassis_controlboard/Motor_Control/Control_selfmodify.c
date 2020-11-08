#include "Motor_Control.h"
#include "Control_selfmodify.h"
#include "fuzzy.h"
#include "motor_driver.h"
#include <string.h>

float error_now[3];
float error_derivative[3];
float origin_left_P = 0;
float origin_left_I = 0;
float origin_left_D = 0;
float origin_right_P = 0;
float origin_right_I = 0;
float origin_right_D = 0;
float origin_Angular_P = 0;
float origin_Angular_I = 0;
float origin_Angular_D = 0;

Selfmod_PID_Typedef Selfmod_LeftWheel_PID;
Selfmod_PID_Typedef Selfmod_RightWheel_PID;
Selfmod_PID_Typedef Selfmod_Angular_PID;

void Init_FuzzyPID(void) // 只在初始化和上位机下达修改PID参数时产生改变
{
    // 左电机PID基础值
    origin_left_P = LeftWheel_PID.KP;
    origin_left_I = LeftWheel_PID.KI;
    origin_left_D = LeftWheel_PID.KD;
    // 右电机PID基础值
    origin_right_P = RightWheel_PID.KP;
    origin_right_I = RightWheel_PID.KI;
    origin_right_D = RightWheel_PID.KD;

    origin_Angular_P = Angular_PID.KP;
    origin_Angular_I = Angular_PID.KI;
    origin_Angular_D = Angular_PID.KD;
}

void FuzzysetPIDPara(ControlData_Typedef *controlDataInst)
{
    Update_Para(controlDataInst);

    fuzzyout(&SpeedTAB, error_now[0], error_derivative[0], &Selfmod_LeftWheel_PID.KP, &Selfmod_LeftWheel_PID.KI, &Selfmod_LeftWheel_PID.KD);

    fuzzyout(&SpeedTAB, error_now[1], error_derivative[1], &Selfmod_RightWheel_PID.KP, &Selfmod_RightWheel_PID.KI, &Selfmod_RightWheel_PID.KD);

    fuzzyout(&SpeedTAB, error_now[2], error_derivative[2], &Selfmod_Angular_PID.KP, &Selfmod_Angular_PID.KI, &Selfmod_Angular_PID.KD);

    if(controlDataInst->Fuzzy_Control_Enable_flag == 1)
    {
        LeftWheel_PID.KP = origin_left_P + 0.7f * Selfmod_LeftWheel_PID.KP;
        LeftWheel_PID.KI = origin_left_I + 0.5f * Selfmod_LeftWheel_PID.KI;
        LeftWheel_PID.KD = origin_left_D + Selfmod_LeftWheel_PID.KD;

        RightWheel_PID.KP = origin_right_P + 0.7f * Selfmod_RightWheel_PID.KP;
        RightWheel_PID.KI = origin_right_I + 0.5f * Selfmod_RightWheel_PID.KI;
        RightWheel_PID.KD = origin_right_D + Selfmod_RightWheel_PID.KD;

//		Angular_PID.KP = origin_Angular_P + 0.5f * Selfmod_Angular_PID.KP;
//		Angular_PID.KI = origin_Angular_I + 0.5f * Selfmod_Angular_PID.KI;
//		Angular_PID.KD = origin_Angular_D + 0.5f * Selfmod_Angular_PID.KD;
    }
}

void Update_Para(ControlData_Typedef *controlDataInst)
{
    error_derivative[0] = (LeftWheel_PID.err[0] - LeftWheel_PID.err[1]) / 0.01f;
    error_derivative[1] = (RightWheel_PID.err[0] - RightWheel_PID.err[1]) / 0.01f;
    error_derivative[2] = (Angular_PID.err[0] - Angular_PID.err[1]) / 0.01f;
    error_now[0] = controlDataInst->wheelSpeed[0] - controlDataInst->present_v[0];
    error_now[1] = controlDataInst->wheelSpeed[1] - controlDataInst->present_v[1];
    error_now[2] = controlDataInst->goalAngular_v_pre - controlDataInst->presentAngular_v;
}
