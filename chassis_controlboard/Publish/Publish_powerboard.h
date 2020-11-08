#ifndef __PUBLISH_POWERBOARD_H
#define __PUBLISH_POWERBOARD_H
#include "stm32f4xx_hal.h"
#include "OD.h"
#include "string.h"
#include "Clock.h"

typedef enum {
	break_power = 1,
  pad_power,
	tx2_power,
	v12_power,
	all_power,
	ult_power,//v12_power 一个接口
}reboot_power;
uint8_t Send_BmsInfo(void);
void Reboot_Power(uint8_t power_type,uint8_t power_status);
#endif
