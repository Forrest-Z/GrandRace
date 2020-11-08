#ifndef _SYS_STACK_H
#define _SYS_STACK_H
#include "cmsis_os.h"
#include <stdint.h>


#pragma pack(1)

typedef struct
{
    uint16_t			stack_sensor;	//传感器任务剩余堆栈量
    uint16_t 			stack_control;//控制任务剩余堆栈量
    uint16_t 			stack_comm;		//通讯任务剩余堆栈量
    uint16_t      stack_fsm;		//状态机任务剩余堆栈量
    uint16_t      stack_log;		//日志任务剩余堆栈量
    uint16_t  		stack_res[3];	//预留
} Sys_Stack_TypeDef;

extern osThreadId CommunicationHandle;
extern osThreadId SensorHandle;
extern osThreadId ControlHandle;
extern osThreadId StateMachineHandle;
extern osThreadId LOGTaskHandle;
void System_Stack_Read(Sys_Stack_TypeDef *pstack);

#endif

