#ifndef _SYS_CPU_H_
#define _SYS_CPU_H_

#include <stdint.h>

extern volatile uint32_t OSIdleCount;

#define MAX_CPU_TIME_CNT			13173 //14267			// 系统延时10ms 统计计数器值

void initSystemCpuPara(void);
void readSystemCpuPara(void);
uint8_t getSystemCpuPara(void);

#endif

