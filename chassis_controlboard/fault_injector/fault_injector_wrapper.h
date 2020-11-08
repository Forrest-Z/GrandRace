/**************************************************************************//**
 * @file     fault_injector_wrapper.h
 * @brief    故障注入接口
 * @version  V2.0
 * @date     2019-04-25
 * @author   ycy
 ******************************************************************************/
#ifndef __FAULT_INJECTOR_WRAPPER_H__
#define __FAULT_INJECTOR_WRAPPER_H__

#include "fault_injector.h"
#include "chassis_config.h"
#include "Clock.h"

#ifdef __TP_USER

void fault_injector_init(void);
bool create_tp(tp tp_id);
bool enable_tp(tp tp_id,uint32_t tp_cnt);
bool disable_tp(tp tp_id,uint32_t tp_cnt);
bool isCanRunTp(tp tp_id);
eTP_status getOneStatus(tp tp_id,bool isPrint);
void getAllStatus_tp(void);
void setStackOver(void);
void memLeak(void);

/*************************************************
Description:		注册故障TP
*************************************************/
#define CREATE_TP(tp_id) create_tp(tp_id)


/*************************************************
Description:		设置故障点tpId生效
*************************************************/
#define ENABLE_TP(tp_id,tp_cnt) enable_tp(tp_id,tp_cnt)

/*************************************************
Description:		设置故障点tpId为无效
*************************************************/
#define DISABLE_TP(tp_id,tp_cnt) disable_tp(tp_id,tp_cnt)
/*************************************************
Description:		在故障点tpId直接返回
*************************************************/
#define TP_RETURN(tp_id)   \
do \
{ \
  if (eTP_ENABLED == getOneStatus(tp_id,false)) { \
      return; \
  }; \
} while (0); 

/*************************************************
Description:		在故障点tpId直接返回特定retVal值
*************************************************/
#define TP_RETURN_WITH(tp_id,retVal) \
do \
{ \
    if (eTP_ENABLED == getOneStatus(tp_id,false)) { \
        return retVal;  \
    }   \
} while (0);

/*************************************************
Description:		在故障点tpId  当前变量 variable 为一个新值
*************************************************/
#define TP_MODIFY_VALUE(tp_id, variable, newVal) \
{ \
    if (isCanRunTp(tp_id)) \
    {   \
        (variable) = newVal; \
    }   \
}

/*************************************************
Description:		在该故障点增加时延
*************************************************/
#define TP_WAIT_MS(tp_id, time_ms) \
{ \
    if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
        delayMs(time_ms); \
    }   \
}

/*************************************************
Description:		在该故障点进入死循环
*************************************************/
#define TP_LOOP(tp_id) \
{ \
		if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
          while(1); \
    }   \
}
/*************************************************
Description:		在该故障点让进程崩溃 1/0 assert(1 == 0); \
*************************************************/
#define TP_EXIT_PROCESS(tp_id) \
{ \
		int a[3]; \
		if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
				  a[10000000]=10; \
    }   \
}

/*************************************************
Description:系统复位 __disable_irq();\ HAL_NVIC_SystemReset();\
*************************************************/
#define TP_SYS_REBOOT(tp_id) \
{ \
		if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
					chassisReboot();\
    }   \
}

/*************************************************
Description:堆栈溢出
*************************************************/
#define TP_STACK_OVER(tp_id) \
{ \
		if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
					setStackOver();\
    }   \
}
/*************************************************
Description:内存泄露 memLeak
*************************************************/
#define TP_MEM_LEAK(tp_id) \
{ \
		if (eTP_ENABLED == getOneStatus(tp_id,false)) \
    {   \
					memLeak();\
    }   \
}

#endif


#endif

