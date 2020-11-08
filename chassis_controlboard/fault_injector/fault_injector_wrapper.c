/**************************************************************************
* @file     fault_injector_wrapper.c
* @brief    故障注入接口
* @version  V2.0
* @date     2019-04-25
* @author   ycy
******************************************************************************/
#include "fault_injector.h"
#include "fault_injector_wrapper.h"
#include <stdlib.h>
#include "freertos.h"
#include "semphr.h"


#ifdef __TP_USER
static uint8_t tpLock = TP_UNLOCK;
static  bool  bFaultInit = false;
/* 互斥信号量句柄 */
//SemaphoreHandle_t m_xSem = NULL;

/*************************************************
Function: 			fault_injector_init
Description:		初始化
Input:					none
Output: 				none
Return: 				none
*************************************************/
void fault_injector_init(void)
{
    /* 创建互斥信号量 */
    //m_xSem = xSemaphoreCreateMutex();
    initFault();
		bFaultInit = true;
}

/*************************************************
Function:			 enable_tp
Description: 	 注册故障ID.
Input: 				 tp_id:故障点ID
Output:				 none
Return:				 bool
*************************************************/
bool create_tp(tp tp_id)
{
    return createFault(tp_id);
}


/*************************************************
Function:			 enable_tp
Description: 	 激活该故障注入点.
Input: 				 tp_id:故障点ID
Output:				 none
Return:				 bool
*************************************************/
bool enable_tp(tp tp_id,uint32_t tp_cnt)
{
    bool ret = false;
    //xSemaphoreTake( m_xSem, portMAX_DELAY );
    ret = enable(tp_id,tp_cnt);
   // xSemaphoreGive( m_xSem );
    return ret;
}

/*************************************************
Function:			 disable_tp
Description: 	 屏蔽该故障注入点.
Input: 				 tp_id:故障点ID
Output:				 none
Return:				 none
*************************************************/
bool disable_tp(tp tp_id,uint32_t tp_cnt)
{
    bool ret = false;
    //xSemaphoreTake( m_xSem, portMAX_DELAY );
    ret =  disable(tp_id,tp_cnt);
    //xSemaphoreGive( m_xSem );
    return ret;
}


/*************************************************
Function:			 getTPStatus
Description: 	 获取某个点的状态.
Input: 				 tp_id:故障ID
Output:				 none
Return:				 none
*************************************************/
eTP_status getOneStatus(tp tp_id,bool isPrint)
{
    eTP_status etp;
   if((tpLock == TP_LOCK) || (bFaultInit == false) ) 
        return eTP_INVALID;

    etp = getStatus(tp_id,isPrint);

    return etp;
}
/*************************************************
Function:			 isCanRunTp
Description: 	 该tp点计数是否到0 是否继续
Input: 				 tp_id:故障ID
Output:				 none
Return:				 none
*************************************************/
bool isCanRunTp(tp tp_id)
{
    bool ret = false;
    if(tpLock==TP_UNLOCK)  
      ret = isCanBeExecTp(tp_id);  
    return  ret;
}

/*************************************************
Function:			 getAllStatus_tp
Description: 	 获取所有点点的状态.
*************************************************/
void getAllStatus_tp(void)
{
    tpLock = TP_LOCK;
    getAllStatus();
    tpLock = TP_UNLOCK;
}

/*************************************************
Function:			 setStackOver
Description: 	 堆栈溢出.构造很大的局部空间
*************************************************/
void setStackOver(void)
{
//    uint32_t sieve[2000000];
//    sieve[22] =1;
    //print_dbg("%d\n",sieve[22]);
    return ;
}
#if 0
/*************************************************
Function:			 allocate_mem
Description: 	 内存泄露
Input: 				 none
Output:				 none
Return:				 none
*************************************************/
int *getMem()
{
    return (int *)calloc(1000,4);
}

void memLeak(void)
{
    getMem();
    //printf(str);
    //free(str);
    return ;
}

#endif
#endif

