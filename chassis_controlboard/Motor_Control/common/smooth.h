#ifndef __SMOOTH_h__
#define __SMOOTH_h__
#include "Motor_Control.h"
extern uint8_t dec_flag;
extern uint8_t aec_flag;
extern uint8_t acc_flag;
extern uint8_t turnleft_flag;
extern uint8_t turnright_flag;
extern uint8_t first_turnleft_flag;
extern uint8_t first_turnright_flag;
void LineVelocitySmooth(ControlData_Typedef *controlData,float diff_time);
void AngularSpeedSmooth(ControlData_Typedef *controlData,float diff_time);
#endif
