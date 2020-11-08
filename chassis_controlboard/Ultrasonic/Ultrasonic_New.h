#ifndef __ULTRASONIC_NEW_H
#define __ULTRASONIC_NEW_H

#include <stdint.h>

#pragma pack(1)

#define ULTRASONIC_MAX_CNT		120

typedef struct
{
    uint8_t count;
    uint8_t databuf[ULTRASONIC_MAX_CNT*3];
} stUltrasonicData;

uint16_t getUltrasonicState(void);

void receiveUltrasonicSampleData(uint8_t group_cnt, uint8_t *pmsg);
void pubUltrasonicSampleData(void);

void getUltrasonicChannelState(void);

void ult_VersionCallback(void);
void ult_ChannelStatusCallback(void);


#endif

