#include "watchdog_task.h"
#include <string.h>
#include "Clock.h"
#include "chassis_config.h"

#ifdef __WDG
#include "iwdg.h"

#ifdef __TP_USER
	#include "fault_injector_wrapper.h"
#endif
	
#endif

//IWDG_HandleTypeDef hiwdg;
EventGroupHandle_t xCreatedEventGroup;
volatile static uint8_t Reset_State = 0;

void initSystemWatchdog(void)
{
#ifdef __WDG
    Reset_State = __HAL_RCC_GET_RESET_FLAGS(RCC_IWDG_RSTF);
    __HAL_RCC_CLEAR_RESET_FLAGS();

    xCreatedEventGroup = xEventGroupCreate();
    if(xCreatedEventGroup == NULL)
    {
        return;
    }
#endif
}

uint8_t getResetState(void)
{
    return Reset_State;
}

void ClearResetState(void)
{
    Reset_State = 0;
}

void Watchdog_Task_Start(void)
{
#ifdef __WDG
    MX_IWDG_Init();
#endif
}

void Watchdog_SectorErase_Reconfigure(void)
{
#ifdef __WDG
    // Tout = (Prescaler * Reload) / 32    	(ms)
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
    hiwdg.Init.Reload = 4095;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
#endif
}

void Watchdog_Task_Feed(uint8_t task_bit)
{

#ifdef __WDG
	#ifdef __TP_USER
		TP_RETURN(eTP14_wdg_rest);
	#endif
    xEventGroupSetBits(xCreatedEventGroup, task_bit);
#endif

}


void Watchdog_Task_Check(void)
{
#ifdef __WDG
    EventBits_t uxBits;
    const TickType_t xTicksToWait = 10;	//100 / portTICK_PERIOD_MS;

    uxBits = xEventGroupWaitBits(xCreatedEventGroup, TASK_BIT_ALL, pdTRUE, pdTRUE, xTicksToWait);

    if((uxBits & TASK_BIT_ALL) == TASK_BIT_ALL)
    {
        HAL_IWDG_Refresh(&hiwdg);
    }

#endif
}



