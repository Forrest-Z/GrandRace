/*	金瑞麒
	吴钦文
	2018/7/4
*/
#ifndef _BLD_MOTOR_DEIVER_H
#define _BLD_MOTOR_DEIVER_H

#include "stm32f4xx_hal.h"
#include "main.h"
#include "math.h"
#include "tim.h"
//#include "motor_driver.h"
#include "control_config.h"
#include "Motor_Control.h"
/*内部函数
***************************************************************/
//电机方向 1正0反
#define DIR_R_MOTOR(x)  x?HAL_GPIO_WritePin(C_R_FR_GPIO_Port,C_R_FR_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(C_R_FR_GPIO_Port,C_R_FR_Pin,GPIO_PIN_RESET)
#define DIR_L_MOTOR(x)  x?HAL_GPIO_WritePin(C_L_FR_GPIO_Port,C_L_FR_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(C_L_FR_GPIO_Port,C_L_FR_Pin,GPIO_PIN_RESET)

//EN 1有效
#define EN_R_MOTOR(x)  x?HAL_GPIO_WritePin(C_R_EN_GPIO_Port,C_R_EN_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(C_R_EN_GPIO_Port,C_R_EN_Pin,GPIO_PIN_SET)
#define EN_L_MOTOR(x)  x?HAL_GPIO_WritePin(C_L_EN_GPIO_Port,C_L_EN_Pin,GPIO_PIN_RESET):HAL_GPIO_WritePin(C_L_EN_GPIO_Port,C_L_EN_Pin,GPIO_PIN_SET)

//BRK 1刹车 0不刹车
#define BRK_R_MOTOR(x)  x?HAL_GPIO_WritePin(C_R_BRK_GPIO_Port,C_R_BRK_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(C_R_BRK_GPIO_Port,C_R_BRK_Pin,GPIO_PIN_RESET)
#define BRK_L_MOTOR(x)  x?HAL_GPIO_WritePin(C_L_BRK_GPIO_Port,C_L_BRK_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(C_L_BRK_GPIO_Port,C_L_BRK_Pin,GPIO_PIN_RESET)

//ALM
#define ALM_R  HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_3)
#define ALM_L  HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)


/*对外的函数
*****************************************************************/
void BLD_IO_Init(MOTORCONTROL_Typedef *pmsg);
void BLD_IO_Write(MOTORCONTROL_Typedef *pmsg);
void BLD_IO_Read(MOTORCONTROL_Typedef *pmsg);
void BLD_IO_Stop(void);
/*end************************************************************/


void setPWM_Motor(char ch,float pwm);//pwm的范围是-1~1 相当于-100%~+100%


//uint8_t brake_Motor(float &speedGoalLeft,float &speedGoalRight);

//初始回调
void BLD_IO_TIM_Base_MspInit_Callback(TIM_HandleTypeDef* htim_base);
//电机速度捕获回调
void BLD_IO_LeftSpeedCaptureCallback(TIM_HandleTypeDef *htim);
void BLD_IO_RightSpeedCaptureCallback(TIM_HandleTypeDef *htim);
void BLD_IO_LeftSpeedPeriodElapsedCallback(TIM_HandleTypeDef *htim);
void BLD_IO_RightSpeedPeriodElapsedCallback(TIM_HandleTypeDef *htim);
/*end************************************************************/

#endif

