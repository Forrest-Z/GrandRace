/**
*此文件只能被包含一次，故不写头文件
*此文件用于调试进入HardFalt的原因
*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "hard_fault_debug.h"

enum { r0=1, r1, r2, r3, r12, lr, pc, psr};


__ASM void HardFaultASM(void)
{
    IMPORT HardFaultC
    TST lr,#4
    ITE EQ
    MRSEQ r0, PSP
    MRSNE r0, MSP
    B HardFaultC
}

void HardFaultC(uint32_t stack[])
{

//	static uint32_t *ptr;
//	static uint32_t buf[8];
//
//	ptr = stack;
//
//	for(int i = 0;i<8;i++)
//	{
//		buf[i] = stack[i];
//	}

//	printf("back point:%08x\n",stack[pc]);
//	printf("function called:%08x\n\n",stack[lr]);
//	__ASM volatile("BKPT #01");
}
