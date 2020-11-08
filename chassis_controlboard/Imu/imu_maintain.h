
/*
*wqw 2019/3/6
*此代码用于维护imu的状态，包括陀螺仪及角速度计校准
*/


/*
1.验证flash接口
2.验证上下位机接口
3.设计代码，注意分层
*/
#ifndef _IMU_MAINTAIN_H
#define _IMU_MAINTAIN_H

#include <stdlib.h>
#include "stm32f4xx_hal.h"
#define GYRO_INIT_TIME 3   //陀螺仪校准零漂的记录时间 (s)

int Run_Gyro_Calibraton(int16_t *Gyro,float temp);
int Run_Gyro_Calibraton2(int16_t *Gyro,float temp);
uint8_t Get_Gyro_Offset(float gyroOffset[4]);
uint8_t Get_Gyro_Offset2(float gyroOffset[3]);
void Begin_Gyro_Calibration(void);

float gyro_CalDeltaOffset(float temp);
#endif

