#include "Clock.h"
#include "cmsis_os.h"

uint16_t Clock_Count = 0;
RTime_TypeDef RealTime = {0,0,0,0,0,0,0};

// 初始化系统时间
void initSystemClock(void)
{
    HAL_TIM_Base_Start_IT(&htim7);
    __HAL_TIM_ENABLE(&htim7);
}

uint32_t readSystemClock(void)
{
    return (((uint32_t)Clock_Count * 65536) + TIM7->CNT);
}

void getSystemTimeStamp(unsigned long int seq, Time_StampTypeDef *timeStap)
{
	static unsigned long long ms_ex = 0;//（单位ms）用一个超长整形维护ms计算器。总溢出时间约为5.99亿年。若32位，溢出时间为49.7天
	static unsigned long ms_buf_last; //会发生翻转的ms时间
	unsigned long ms_buf;
	
	ms_buf = HAL_GetTick();
	if(ms_buf_last>ms_buf)//如果发生溢出
	{
		ms_ex += 0x100000000; // 每49.7天发生一次
	}
	ms_buf_last = ms_buf;
	
	timeStap->sec = (ms_ex + ms_buf)/1000;
	timeStap->nsec = (ms_ex + ms_buf)%1000*1000000;	//ns部分
	timeStap->seq = seq; //seq由每个单独的发送topic的代码维护
}


/*2019/8/26 wqw
时间戳由Time7改为滴答定时器，以前的代码弃用
******************************************************************/
#if 0
void getSystemTimeStamp1(unsigned long int seq, Time_StampTypeDef *t)
{
    static unsigned long int us = 0;
    static unsigned long int t1 = 0, t2 = 0, val = 0;

    us = readSystemClock();
    t1 = (us / 1000000);
    if(t1 < t2)
    {
        val += t2;
        t2 = 0;
    }
    else
    {
        t2 = t1;
    }

    t->sec  = val + t1;
    t->nsec = (us % 1000000) * 1000;
    t->seq  = seq;
}
#endif
uint32_t getSystemTick(void)
{
    return HAL_GetTick();
}

void delayUs(uint32_t us)
{
    uint32_t target_time = (readSystemClock() + us);

    while(1)
    {
        if(readSystemClock() > target_time)
        {
            break;
        }
    }
}

void delayMs(uint32_t ms)
{
    //Driver_delayUs(ms * 1000);
    osDelay(ms);
}

void delaySec(uint32_t s)
{
    delayMs(s * 1000);
}

void writeSystemTime(uint8_t *time)
{
    if(time == NULL)
        return;

    RealTime.year = time[0];
    RealTime.mon  = time[1];
    RealTime.day  = time[2];
    RealTime.hour = time[3];
    RealTime.min  = time[4];
    RealTime.sec  = time[5];
    RealTime.us_sec = (time[6] << 24) + (time[7] << 16) + (time[8] << 8) + time[9];
}

void getSystemTime(RTime_TypeDef *time)
{
    if(time == NULL)
        return;

    memcpy(time, &RealTime, sizeof(RTime_TypeDef));
}
