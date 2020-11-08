#ifndef M_C_MONITOR_H
#define M_C_MONITOR_H

#include "tim.h"
#include "Motor_Control.h"
void M_C_MonitorInit(ControlData_Typedef *controlData);

void M_C_MonitorInterface(ControlData_Typedef *controlData);

void M_C_MonitorCallback(TIM_HandleTypeDef *htim);

void M_C_MonitorLockPower(ControlData_Typedef *controlData);

#endif

