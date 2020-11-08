#ifndef _WATCHDOG_TASK_H
#define _WATCHDOG_TASK_H

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"



#define	TASK_BIT_0			(0x01 << 0)
#define	TASK_BIT_1			(0x01 << 1)
#define	TASK_BIT_2			(0x01 << 2)
#define	TASK_BIT_3			(0x01 << 3)
#define	TASK_BIT_4			(0x01 << 4)
#define	TASK_BIT_ALL		(TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2 | TASK_BIT_3 | TASK_BIT_4)
//#define	TASK_BIT_ALL		(TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_2)

#define RCC_IWDG_RSTF 	0x20
#define __HAL_RCC_GET_RESET_FLAGS(__FLAG__) (((RCC->CSR >> 24) & (__FLAG__)) == (__FLAG__))
#define __HAL_RCC_GET_RSTCSR()							(( RCC->CSR >> 24) & 0xFF)



uint8_t getResetState(void);
void ClearResetState(void);
void initSystemWatchdog(void);
void Watchdog_Task_Start(void);
void Watchdog_SectorErase_Reconfigure(void);
void Watchdog_Task_Feed(uint8_t task_bit);
void Watchdog_Task_Check(void);
//void Watchdog_Feed_Log(void);

#endif
