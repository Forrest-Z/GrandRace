#include "sys_stack.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"

enum { r0=1, r1, r2, r3, r12, lr, pc, psr};


void System_Stack_Read(Sys_Stack_TypeDef *pstack)
{
    pstack->stack_sensor  = uxTaskGetStackHighWaterMark(SensorHandle);
    pstack->stack_control = uxTaskGetStackHighWaterMark(ControlHandle);
    pstack->stack_comm    = uxTaskGetStackHighWaterMark(CommunicationHandle);
    pstack->stack_fsm     = uxTaskGetStackHighWaterMark(StateMachineHandle);
    pstack->stack_log     = uxTaskGetStackHighWaterMark(LOGTaskHandle);
}

