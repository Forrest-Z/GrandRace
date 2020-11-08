
#ifndef _MOTOR_DRIVER_COMMON_H
#define _MOTOR_DRIVER_COMMON_H

#include "control_config.h"

//电磁刹车 1刹车 0松开刹车
#define EN_Lock_MOTOR(x) x?HAL_GPIO_WritePin(BREAK_GPIO_Port,BREAK_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(BREAK_GPIO_Port,BREAK_Pin,GPIO_PIN_SET)

//急停开关状态
#define SCRAM_STATE (!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4))

void Motor_Driver_Common_Write(MOTORCONTROL_Typedef *pmsg);

#endif

