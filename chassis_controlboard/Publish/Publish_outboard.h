#ifndef __PUBLISH_OUTBOARD_H
#define __PUBLISH_OUTBOARD_H
#include "stm32f4xx_hal.h"
#include "OD.h"
#include "string.h"
#include "Clock.h"

//led
// 接口 cmd 接收到led控制数据 cmd 
typedef struct _LED_CMD_TypeDef
{
    uint8_t     cmd; 			// 命令接口 
    uint32_t    onoff_ms;	//亮灭时间
	  uint32_t    hz_ms;		//间隔时间
} LedCmd;

// 接口 cmd 接收到led控制数据 cmd 
typedef struct _ULT_CH_TypeDef
{
    uint8_t     ch; 			//通道接口 
    uint8_t     en;	      //通道使能
} UltCh;


//灯带
extern LedCmd sLedCmd;
void sendLedCtl(LedCmd sLed);
void sendLedCtl_fault(void);
void sendLedCtl_ChassisInit(void);

//摇杆
extern uint8_t reset_joy;
extern uint8_t Joy_SubReset;
void sendJoy_reset(void);
void Joy_resetCallback(void);
extern UltCh sSetUltCh;
extern UltCh sGetUltCh;
extern UltCh g_Ult_status;

void sendSetUltCh(UltCh sUlt);
void sendGetUltCh(UltCh sUlt);


#endif

