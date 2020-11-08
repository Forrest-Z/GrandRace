#ifndef __CLOCK_H
#define __CLOCK_H
#include "tim.h"
#include "stm32f4xx_hal.h"
#include "protocol.h"
#include <string.h>

#pragma pack(1)

typedef struct __RTIME
{
    uint8_t year;
    uint8_t mon;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint32_t us_sec;
} RTime_TypeDef;

extern uint16_t Clock_Count ;
uint32_t readSystemClock(void);
void initSystemClock(void);
void getSystemTimeStamp(unsigned long int seq, Time_StampTypeDef *t);
uint32_t getSystemTick(void);

void delayUs(uint32_t us);
void delayMs(uint32_t ms);
void delaySec(uint32_t s);

void writeSystemTime(uint8_t *time);
void getSystemTime(RTime_TypeDef *time);

#endif

