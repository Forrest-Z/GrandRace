/***************************************************
 * @file     trmp_sensor.h
 * @brief    读取CPU内部温度。
 * @version  V2.9
 * @date     2019-08-09
 * @author   wqw
 ****************************************************/
#ifndef _TEMP_SENSOR_H_
#define _TEMP_SENSOR_H_
#include "adc.h"

typedef struct
{
	float M4;   //片内温度
	float IMU;  //imu 温度
 //为0后M4的值有效 因为需要采内部基准电压，
 //所以开机后10s内M4温度是无效的。count会将它指示出来
	unsigned count;		//秒速计数初始值10
}Temp_Typedef;

void init_temp_sensor(void);
void read_temp_sensor(void);
void get_temp_sensor(Temp_Typedef* temp_ptr);
uint8_t getM4Temp(void);
uint8_t getImuTemp(void);

#endif

