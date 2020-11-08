/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "info_core.h"
#include "Motor_Control.h"
#include "Mb_Master.h"
#include "Mb_crc16.h"
#include "sys_cpu.h"
#include "Publish_Pc.h"
#include "outboard_sensor.h"
#include "log_app.h"
#include "watchdog_task.h"
#include "tx2_power.h"
#include "tcp_demo.h"
#include "chassis_config.h"
#include "../statemachine/chassis_fsm.h"
#include "SD_Log_App.h"

uint8_t test_task;
uint32_t task_time_test;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TickType_t time_test[3] = {0};
/* USER CODE END Variables */
osThreadId CommunicationHandle;
osThreadId SensorHandle;
osThreadId ControlHandle;
osThreadId StateMachineHandle;
osThreadId LOGTaskHandle;
osMessageQId fsmEvtQueueHandle;
osMessageQId fsmPubQueueHandle;
osMessageQId chassisEvtQueueHandle;
osMutexId RAM_BUSYHandle;
osMutexId SD_BUSYHandle;
osMutexId COM_BUSYHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osMutexId TCP_busyHandle;
osSemaphoreId creatfullHandle;
/* USER CODE END FunctionPrototypes */

void CommunicationTask(void const * argument);
void SensorTask(void const * argument);
void ControlTask(void const * argument);
void StateMachineTask(void const * argument);
void SDLOG(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
__weak void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
    to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
    task. It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()). If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted. */
    taskENTER_CRITICAL();
    OSIdleCount++;
    taskEXIT_CRITICAL();
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Create the mutex(es) */
  /* definition and creation of RAM_BUSY */
  osMutexDef(RAM_BUSY);
  RAM_BUSYHandle = osMutexCreate(osMutex(RAM_BUSY));

  /* definition and creation of SD_BUSY */
  osMutexDef(SD_BUSY);
  SD_BUSYHandle = osMutexCreate(osMutex(SD_BUSY));

  /* definition and creation of COM_BUSY */
  osMutexDef(COM_BUSY);
  COM_BUSYHandle = osMutexCreate(osMutex(COM_BUSY));

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
	osMutexDef(TCP_busy);
  TCP_busyHandle = osMutexCreate(osMutex(TCP_busy));
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
	osSemaphoreDef(creatfull);
	creatfullHandle = osSemaphoreCreate(osSemaphore(creatfull), 1);
	osSemaphoreWait(creatfullHandle,1000);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of Communication */
  osThreadDef(Communication, CommunicationTask, osPriorityBelowNormal, 0, 1024);
  CommunicationHandle = osThreadCreate(osThread(Communication), NULL);

  /* definition and creation of Sensor */
  osThreadDef(Sensor, SensorTask, osPriorityNormal, 0, 896);
  SensorHandle = osThreadCreate(osThread(Sensor), NULL);

  /* definition and creation of Control */
  osThreadDef(Control, ControlTask, osPriorityBelowNormal, 0, 640);
  ControlHandle = osThreadCreate(osThread(Control), NULL);

  /* definition and creation of StateMachine */
  osThreadDef(StateMachine, StateMachineTask, osPriorityAboveNormal, 0, 640);
  StateMachineHandle = osThreadCreate(osThread(StateMachine), NULL);

  /* definition and creation of LOGTask */
  osThreadDef(LOGTask, SDLOG, osPriorityAboveNormal, 0, 1536);
  LOGTaskHandle = osThreadCreate(osThread(LOGTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of fsmEvtQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(fsmEvtQueue, 64, uint16_t);
  fsmEvtQueueHandle = osMessageCreate(osMessageQ(fsmEvtQueue), NULL);

  /* definition and creation of fsmPubQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(fsmPubQueue, 16, uint16_t);
  fsmPubQueueHandle = osMessageCreate(osMessageQ(fsmPubQueue), NULL);

  /* definition and creation of chassisEvtQueue */
/* what about the sizeof here??? cd native code */
  osMessageQDef(chassisEvtQueue, 32, uint16_t);
  chassisEvtQueueHandle = osMessageCreate(osMessageQ(chassisEvtQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
//		comm_cnt = 0;
//		sensor_cnt = 0;
//		contrl_cnt = 0;
//		state_cnt = 0;
//		log_cnt = 0;
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_CommunicationTask */
/**
  * @brief  Function implementing the Communication thread.
  * @param  argument: Not used
  * @retval None
  */
#define  TASK_DELAY_TIME_MS 50
#define  LOG_WAIT_TICK 21000


/* USER CODE END Header_CommunicationTask */
void CommunicationTask(void const * argument)
{

  /* USER CODE BEGIN CommunicationTask */
    static TickType_t xLastCommTime = 0;
  
    Watchdog_Task_Feed(TASK_BIT_2);
    osDelay(TASK_DELAY_TIME_MS);
    
    //等待SD初始
		while(get_fatfs_initstatus_app() != FATFS_INITOK)
		{
			Watchdog_Task_Feed(TASK_BIT_ALL);
			osDelay(TASK_DELAY_TIME_MS);
      if(getSystemTick() > LOG_WAIT_TICK ) break;
		}
    
		GR_LOG_INFO("==CommunTask init ok ==");
    /* Infinite loop */
    for(;;)
    {
				test_task = 1;
//				comm_cnt++;
        xLastCommTime = xTaskGetTickCount();
        Watchdog_Task_Feed(TASK_BIT_2);
        infoSpinOnceHanlder();
        infoPublishHanlder();
        handlerTx2PowerOff();

// 使用网口通信
#ifdef __COMMUN_USER_NET
        do_tcp_server();
        get_constatus(&tcp_constatus);
        Ir_status = getSn_IR(SOCK_TCPS);
#endif

        time_test[2] = xTaskGetTickCount() - xLastCommTime;
				task_time_test = time_test[2];
        vTaskDelayUntil(&xLastCommTime, 10);

    }
  /* USER CODE END CommunicationTask */
}

/* USER CODE BEGIN Header_SensorTask */
/**
* @brief Function implementing the Sensor thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SensorTask */
void SensorTask(void const * argument)
{
  /* USER CODE BEGIN SensorTask */
//	static uint8_t test_id[2] = {0};

    static TickType_t xLastSensorTime = 0;
  
    Watchdog_Task_Feed(TASK_BIT_0);
    osDelay(TASK_DELAY_TIME_MS);
    //等待SD初始
		while(get_fatfs_initstatus_app() != FATFS_INITOK)
		{
			Watchdog_Task_Feed(TASK_BIT_ALL);
			osDelay(TASK_DELAY_TIME_MS);
      
      if(getSystemTick() > LOG_WAIT_TICK ) break;
		}
		GR_LOG_INFO("==Sensor task init ok==");
    /* Infinite loop */
    for(;;)
    {
				test_task = 2;
//				sensor_cnt++;
        xLastSensorTime = xTaskGetTickCount();
        Watchdog_Task_Feed(TASK_BIT_0);
        readSenorDevice();
        time_test[0] = xTaskGetTickCount() - xLastSensorTime;
        vTaskDelayUntil(&xLastSensorTime, 10);
    }
  /* USER CODE END SensorTask */
}

/* USER CODE BEGIN Header_ControlTask */
/**
* @brief Function implementing the Control thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ControlTask */
void ControlTask(void const * argument)
{
  /* USER CODE BEGIN ControlTask */
    static TickType_t xLastControlTime = 0;
  
    Watchdog_Task_Feed(TASK_BIT_1);
    osDelay(TASK_DELAY_TIME_MS);
    
    //等待SD初始
		while(get_fatfs_initstatus_app() != FATFS_INITOK)
		{
			Watchdog_Task_Feed(TASK_BIT_ALL);
			osDelay(TASK_DELAY_TIME_MS);
      
      if(getSystemTick() > LOG_WAIT_TICK ) break;
		}
	
		Init_Motor();	
    /* Infinite loop */
    
    Watchdog_Task_Feed(TASK_BIT_1);
    osDelay(TASK_DELAY_TIME_MS);
		GR_LOG_INFO("==ControlTask init ok==");
    for(;;)
    {
				test_task = 3;
//				contrl_cnt++;
        xLastControlTime = xTaskGetTickCount();
        Watchdog_Task_Feed(TASK_BIT_1);
        control_main(0);
        time_test[1] = xTaskGetTickCount() - xLastControlTime;
        vTaskDelayUntil(&xLastControlTime, 10);
    }
  /* USER CODE END ControlTask */
}

/* USER CODE BEGIN Header_StateMachineTask */
/**
* @brief Function implementing the StateMachine thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StateMachineTask */
void StateMachineTask(void const * argument)
{
  /* USER CODE BEGIN StateMachineTask */
    static TickType_t xLastFsmTime = 0;


    Watchdog_Task_Feed(TASK_BIT_3);
    osDelay(TASK_DELAY_TIME_MS);
    //等待SD初始
		while(get_fatfs_initstatus_app() != FATFS_INITOK)
		{
			Watchdog_Task_Feed(TASK_BIT_ALL);
			osDelay(TASK_DELAY_TIME_MS);
      
      if(getSystemTick() > LOG_WAIT_TICK ) break;
		}
    //initSystemCpuPara();
    initChassisFsmEvt();

		GR_LOG_INFO("==State machine init ok ==");
    /* Infinite loop */
    for(;;)
    {
				test_task = 4;
//				state_cnt++;
        xLastFsmTime = xTaskGetTickCount();
        Watchdog_Task_Feed(TASK_BIT_3);
        Watchdog_Task_Check();
        scheduleChassisFsmEvt();

        vTaskDelayUntil(&xLastFsmTime, 10);
    }
  /* USER CODE END StateMachineTask */
}

/* USER CODE BEGIN Header_SDLOG */
/**
* @brief Function implementing the LOGTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SDLOG */
void SDLOG(void const * argument)
{
  /* USER CODE BEGIN SDLOG */
	vTaskSuspend(CommunicationHandle);
	vTaskSuspend(ControlHandle);
	vTaskSuspend(SensorHandle);
	vTaskSuspend(StateMachineHandle);
	//vTaskSuspendAll();
	SD_App_Init();
  Watchdog_Task_Feed(TASK_BIT_4);
  osDelay(TASK_DELAY_TIME_MS);
      //先喂�?
  Watchdog_SectorErase_Reconfigure();
  //�?查sd初始化是否成�?
  while(get_fatfs_initstatus_app() != FATFS_INITOK)
  {
    if(getSystemTick() > LOG_WAIT_TICK )
    {
      break;
    }
		Watchdog_Task_Feed(TASK_BIT_ALL);
    osDelay(TASK_DELAY_TIME_MS);
    SD_App_Init();
  }
	Log_Init();
	/* Infinite loop */
  if(get_fatfs_initstatus_app() == FATFS_INITOK)
  {
    //扫描文件数量以及创建时间
    scan_files();
  }
  Watchdog_Task_Start();   
	//xTaskResumeAll();
	vTaskResume(CommunicationHandle);
	vTaskResume(ControlHandle);	
	vTaskResume(SensorHandle);	
	vTaskResume(StateMachineHandle);	
  GR_LOG_INFO("==Chassis SD init ok ==");
  for(;;)
  {
		test_task = 5;
//		log_cnt++;
		Watchdog_Task_Feed(TASK_BIT_4);
		logCmdHanlder();
		file_manage_app();
		sync_log();
    osDelay(100);//100ms = 10hz
  }
  /* USER CODE END SDLOG */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
