#include "Publish_outboard.h"
#include "SD_Log_App.h"
#include "chassis_com.h"

uint8_t reset_joy = ON;
uint8_t Joy_SubReset = OFF;


// 接口 cmd 接收到led控制数据 cmd 
LedCmd sLedCmd = {
	18,
	200,
	500,
};
  
UltCh sSetUltCh = {
    0,
    0,
 };
UltCh sGetUltCh = {
    0,
    0,
 };

/*************************************************
Function: 			sendLedCtl
Description:		灯带控制效果命令
Input:	cmd :    sLed 
*************************************************/
void sendLedCtl(LedCmd sLed)
{
	memcpy(&sLedCmd, &sLed, sizeof(LedCmd));
  publishFrameData(LED_ID,PUB_LED_CTRL_ID, (unsigned char *)&sLedCmd, sizeof(LedCmd));
}


/*************************************************
Function:       sendLedCtl_fault
Description:    下位机检测到故障 红灯闪烁 100 500
Input:  cmd :    sLed 
*************************************************/
void sendLedCtl_fault(void)
{
  sLedCmd.cmd = 19;//红灯三闪
  sLedCmd.onoff_ms = 300;// 开关 100ms
  sLedCmd.hz_ms = 1000; //频率 800ms
  publishFrameData(LED_ID,PUB_LED_CTRL_ID, (unsigned char *)&sLedCmd, sizeof(LedCmd));
}
/*************************************************
Function:       sendLedCtl_ChassisInit
Description:    下位机启动初始化完成
Input:  cmd :    sLed 
*************************************************/
void sendLedCtl_ChassisInit(void)
{
  sLedCmd.cmd = 18;//流水灯 18
  sLedCmd.onoff_ms = 100;// 开关 100ms 
  sLedCmd.hz_ms = 100; //频率 800ms
  publishFrameData(LED_ID,PUB_LED_CTRL_ID, (unsigned char *)&sLedCmd, sizeof(LedCmd));
}
/*************************************************
Function:       sendJoy_reset
Description:    标定
Input:  cmd :    sLed 
*************************************************/
void sendJoy_reset(void)
{
  publishFrameData(JOYS_ID,PUB_JOY_RESET_ID, &reset_joy,sizeof(reset_joy));
	sdlog_printf_app("sand reset_joy ...");
  GR_LOG_INFO("sand reset_joy ...");
}

/*************************************************
Function:       Joy_resetCallback
Description:    标定回调
Input:  cmd :    joys 
*************************************************/
void Joy_resetCallback(void)
{
    //1== ok
    if(Joy_SubReset==ON)
    {
  		sdlog_printf_app("set reset_joy ok");
  		GR_LOG_INFO("set reset_joy ok");
    }
    else
    {
  		sdlog_printf_app("set reset_joy err");
  		GR_LOG_ERROR("set reset_joy err");
    }
    Joy_SubReset = OFF;
}


/*************************************************
Function: 			sendUltCh
Description:		设置超声波通道
Input:	cmd :    num  
*************************************************/
void sendSetUltCh(UltCh sUlt)
{
	memcpy(&sSetUltCh, &sUlt, sizeof(UltCh));
  publishFrameData(ULT_ID,PUB_Ult_SET_CHA_ID, (unsigned char *)&sSetUltCh, sizeof(UltCh));
  sdlog_printf_app("send set ult %d %d ok ",sUlt.ch,sUlt.en);
  GR_LOG_INFO("send set ult %d %d ok ",sUlt.ch,sUlt.en);

}
void sendGetUltCh(UltCh sUlt)
{
  sdlog_printf_app("send get ult %d %d",sUlt.ch,sUlt.en);
  GR_LOG_INFO("send get ult %d %d",sUlt.ch,sUlt.en);
	memcpy(&sGetUltCh, &sUlt, sizeof(UltCh));
  publishFrameData(ULT_ID,PUB_Ult_GET_CHA_ID, (unsigned char *)&sGetUltCh, sizeof(UltCh));
}



