#include "sys_cpu.h"
#include "cmsis_os.h"
#include "watchdog_task.h"
#include "Clock.h"
#include "chassis_config.h"
#include "sys_stack.h"

#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

volatile uint32_t OSIdleCount = 0;
volatile static uint32_t OSIdleCtrRun = 0;
volatile static uint8_t cpu_usage = 0;
volatile static uint32_t OSIdleCtrMax = 0;


// 获取单位时间内最大计数器值
void getMaxCpuRunCount(void)
{
    TickType_t xLastCpuTime;

//	Watchdog_Task_Feed(TASK_BIT_ALL);

    xLastCpuTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastCpuTime, 10);

    taskENTER_CRITICAL();
    OSIdleCtrMax = OSIdleCount;
    taskEXIT_CRITICAL();

    if (OSIdleCtrMax == 0)
    {
        cpu_usage = 0;
    }
}

void initSystemCpuPara(void)
{
    vTaskSuspend(CommunicationHandle);
    vTaskSuspend(ControlHandle);
    vTaskSuspend(SensorHandle);
    vTaskSuspend(LOGTaskHandle);
    getMaxCpuRunCount();
    vTaskResume(ControlHandle);
    vTaskResume(CommunicationHandle);
    vTaskResume(SensorHandle);
    vTaskResume(LOGTaskHandle);
}

// 读取系统cpu使用率
void readSystemCpuPara(void)
{
    volatile static uint8_t usage = 0;
    volatile static uint16_t cpu_sum = 0;
    volatile static uint16_t cpu_cnt = 0;

    taskENTER_CRITICAL();
    OSIdleCtrRun = OSIdleCount;
    OSIdleCount  = 0;
    taskEXIT_CRITICAL();

// 通过测试达到的最大单位计数值可用常量代替
//	if(OSIdleCtrRun <= MAX_CPU_TIME_CNT)
//	{
//		usage = ((MAX_CPU_TIME_CNT - OSIdleCtrRun) * 100) / MAX_CPU_TIME_CNT;
//		if(usage <= 100)
//			cpu_usage = usage;
//	}
    if(OSIdleCtrRun <= MAX_CPU_TIME_CNT)
    {
        usage = ((MAX_CPU_TIME_CNT - OSIdleCtrRun) * 100) / MAX_CPU_TIME_CNT;
        if(usage <= 100)
        {
            //cpu_usage = usage;
            cpu_sum += usage;
						
            if(cpu_cnt++ >= (500 - 1))
            {
                cpu_usage = cpu_sum / 500;
                cpu_cnt = 0;
                cpu_sum = 0;
            }
        }
    }
}

// 获取系统cpu使用率
uint8_t getSystemCpuPara(void)
{
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP1_CPU_100,cpu_usage, 100);
		TP_MODIFY_VALUE(eTP2_CPU_90,cpu_usage, 90);
#endif

    return cpu_usage;

	
}
