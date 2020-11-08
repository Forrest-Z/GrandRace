#ifndef _MOTOR_DRIVER_H
#define _MOTOR_DRIVER_H

#include "BLD_IO.h"
#include "AQMDBL.h"
#include "motor_driver_config.h"
#include "KEYA_Can.h"
#include "motor_driver.h"
#include "motor_state.h"

unsigned char initMotorDriver(MOTORCONTROL_Typedef *ctrl_msg);

unsigned char writeMotorControlData(MOTORCONTROL_Typedef *ctrl_msg);
unsigned char readMotorDevice(void);

unsigned char updateMotorData(MOTORCONTROL_Typedef *ctrl_msg);

MOTORCONTROL_Typedef * getMotorInst(void);

uint16_t getMotorState(void);
uint16_t getMotorDriverState(void);

float getMotorTs(void);

void motorDriverCallback(CAN_HandleTypeDef *hcan);

#endif



