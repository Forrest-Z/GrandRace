#ifndef _AQMDBL_H
#define _AQMDBL_H

#include "aqm_motor_driver.h"
#include "control_config.h"

unsigned char AQMDBL_initMotorControlData(MOTORCONTROL_Typedef *ctrl_msg);
unsigned char AQMDBL_writeMotorControlData(MOTORCONTROL_Typedef *ctrl_msg);
unsigned char AQMDBL_readMotorControlData(MOTORCONTROL_Typedef *ctrl_msg);

unsigned char AQMDBL_getMotorState(void);
#endif

