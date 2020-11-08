
#ifndef _MOTOR_STATE_H_
#define _MOTOR_STATE_H_

#include "control_config.h"
#include "alarm_com.h"

//unsigned char getMotorState(void);
//void clearMotorState(void);

////用于获取电机的状态信息
//unsigned int readMotorState(MOTORCONTROL_Typedef *motor_inst);
////用于复位电机的状态信息
//void resetMotorState(MOTORCONTROL_Typedef *pmsg);

unsigned int readMotorState(MOTORCONTROL_Typedef *motor_inst);
void readVoltageState(MOTORCONTROL_Typedef *motor_inst);


void clearMotorState(void);

#endif
