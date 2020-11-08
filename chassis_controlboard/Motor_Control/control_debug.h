#ifndef _CONTROL_DEBUG_H
#define _CONTROL_DEBUG_H
#include "Motor_Control.h"
void updateControlDebugdata(ControlData_Typedef *control_data_Ptr);
void publishControlDebugInfo(void);
void setPIDPara(void);
void printf_motor_I(void);
#endif
