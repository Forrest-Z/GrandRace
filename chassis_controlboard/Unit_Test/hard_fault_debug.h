#ifndef _HARD_FAULT_DEBUG_H
#define _HARD_FAULT_DEBUG_H
#include "stdint.h"


void HardFaultC(uint32_t stack[]);
__ASM void HardFaultASM(void);
#endif

