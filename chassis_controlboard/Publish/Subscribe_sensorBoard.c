#include "Subscribe_sensorBoard.h"
#include "OD.h"
#include <string.h>
#include "chassis_com.h"
#include "SD_Log_App.h"
#include "log_app.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

//连续校验错误时 进入故障状态
static bool bChecksum_err = false ;
Joystick_InfoTypeDef joy_raw_Data =
{
    .coord_x = 0,
    .coord_y = 0,
    .coord_z = 0,
};


/*************************************************
Function: 			recvJoyRawData
Description:		读取摇杆数据，解析协议
Input:					none
Output: 				eJOYS_STATE：摇杆工作状态
Return: 				pmsg ： 摇杆数据 
*************************************************/
void recvJoyRawData(Joystick_InfoTypeDef *pmsg)
{
  uint8_t check;
  static uint8_t cnt = 0,checksumErrCnt = 0;
  static bool bLog = false;
  static bool bLog_cal = false;//标定

  if((joyComBuff.SOI == 0x3A) && (joyComBuff.EOI == 0x7E))// 帧头帧尾判断
  {
    check = (uint8_t)(joyComBuff.coord_x + joyComBuff.coord_y + joyComBuff.coord_z);
#ifdef __TP_USER
		TP_MODIFY_VALUE(eTP54_joy_checksum_err,check,(check+1));
#endif
    if(check == joyComBuff.checksum)
    {
      if(((joyComBuff.coord_x > WARN_TH_JOY_MAX) || (joyComBuff.coord_x < WARN_TH_JOY_MIN))
      || ((joyComBuff.coord_y > WARN_TH_JOY_MAX) || (joyComBuff.coord_y < WARN_TH_JOY_MIN))
      || ((joyComBuff.coord_z > WARN_TH_JOY_MAX) || (joyComBuff.coord_z < WARN_TH_JOY_MIN)))
      {
        if (!bLog)
        {
          GR_LOG_ERROR("checksum ok, but joy data error");
          bLog = true;
        }
        if(cnt++ >= 5)
        {
          cnt = 0;
          memcpy(pmsg, &joyComBuff, sizeof(Joystick_InfoTypeDef));
          return;
        }
        else	//接收低于5次的错误数据进行丢弃
        {
          return;
        }
      }
      cnt = 0;
      setAlarm(eA_EXTE,eb18_joyCheckSumErr_info,ALARM_OK);
      bChecksum_err = false;
      memcpy(pmsg, &joyComBuff, sizeof(Joystick_InfoTypeDef));
      if (bLog)
      {
        GR_LOG_WARN("joy checksum recover [%02x][%02x] ",check,joyComBuff.checksum);
        bLog = false;
      }  
      checksumErrCnt = 0; //校验错误计数清零

#ifdef __TP_USER
      TP_MODIFY_VALUE(eTP63_joyCalibrationErr,joyComBuff.state,ERR);
#endif

      //检测摇杆标定
      if(joyComBuff.state == OK && bLog_cal == true)
      {
          bLog_cal = false;
          setAlarm(eA_EXTE,eb19_joyCalibrationErr,ALARM_OK);
          GR_LOG_INFO("joy Calibration recover");
      }
      else if (joyComBuff.state == ERR && bLog_cal == false)
      { 
          bLog_cal = true;
          setAlarm(eA_EXTE,eb19_joyCalibrationErr,ALARM_ERR);
          GR_LOG_ERROR("joy Calibration error");
      }
    }
    else
    {
        if (!bLog)
        {
          GR_LOG_ERROR("joy checksum error [%02x][%02x] ",check,joyComBuff.checksum);
          bLog = true;
        } 
        setAlarm(eA_EXTE,eb18_joyCheckSumErr_info,ALARM_ERR);
        if(checksumErrCnt++ > 5 )
        {
          bChecksum_err = true;
          checksumErrCnt = 0;
        }
    }
  }
  else
  {
		if (!bLog)
		{
			GR_LOG_ERROR("joy SOI EOI error [%02x][%02x] ",joyComBuff.SOI,joyComBuff.EOI);
			bLog = true;
		}  
    setAlarm(eA_EXTE,eb18_joyCheckSumErr_info,ALARM_ERR);
    if(checksumErrCnt++ > 5 )
    {
      bChecksum_err = true;
      checksumErrCnt = 0;
    }
  }

}
/*************************************************
Function: 			joystickProtocolCallBack
Description:		摇杆通信返回数据接口
Input:					none
Output: 				joy_raw_Data : 接收到数据存放到结构体
*************************************************/
void joystickProtocolCallBack(void)
{
#ifndef __TEST_CASE_EN
    recvJoyRawData(&joy_raw_Data);
#endif
}

/*************************************************
Function: 			getJoystickRawData
Description:		读取摇杆原始数据，是经过通信层的校验了的
Input:					none
Output: 				eJOYS_STATE：摇杆工作状态
Return: 				pmsg ： 摇杆数据 
*************************************************/
uint8_t getJoystickRawData(Joystick_TypeDef *pmsg)
{
		static bool bLog = false;
		static bool bLogCom = false;
		static uint8_t bCnt = 0;
    
    if(getJOYCommunicationState() == 0 || bChecksum_err == true)
    {
        pmsg->val_x = 0;
        pmsg->val_y = 0;
				
				bCnt ++;
			  if(!bLogCom  && (bCnt > 3) )
				{
					GR_LOG_ERROR("Joy communaton error!");
					bLogCom = true;
				}	
				if (bCnt > 200 ) bCnt = 200;
        if(bLogCom == true)
        {
          setAlarm(eA_EXTE,eb01_joysCommunFatal,ALARM_ERR);//摇杆通信断开，默认将所有摇杆告警置1 
          return JOYS_DISCONNECT;
        }
    }
		else
		{
      	if(bLogCom)
				{
					GR_LOG_WARN("Joy communaton recover !");
					bLogCom = false;
				}
				setAlarm(eA_EXTE,eb01_joysCommunFatal,ALARM_OK);
				bCnt = 0;
		}

#ifdef __TP_USER
        TP_MODIFY_VALUE(eTP53_joy_data_err,joy_raw_Data.coord_x,(WARN_TH_JOY_MAX+1));
#endif

    if( ((joy_raw_Data.coord_x > WARN_TH_JOY_MAX) || (joy_raw_Data.coord_x < WARN_TH_JOY_MIN))
     || ((joy_raw_Data.coord_y > WARN_TH_JOY_MAX) || (joy_raw_Data.coord_y < WARN_TH_JOY_MIN))
     || ((joy_raw_Data.coord_z > WARN_TH_JOY_MAX) || (joy_raw_Data.coord_z < WARN_TH_JOY_MIN)))
    {
			  if(!bLog)
				{
					GR_LOG_ERROR("Joy data error! x:%d y:%d z%d",
            joy_raw_Data.coord_x,joy_raw_Data.coord_y,joy_raw_Data.coord_z);
					bLog = true;
				}
        pmsg->val_x = 0;
        pmsg->val_y = 0;

				setAlarm(eA_EXTE,eb00_joysDateErr,ALARM_ERR);
        return JOYS_STATE_ERR; 		
    }
		else
		{
        if(bLog)
				{
					GR_LOG_WARN("Joy data recover !");
          bLog = false;
				}
				setAlarm(eA_EXTE,eb00_joysDateErr,ALARM_OK);
		}

    pmsg->val_x = joy_raw_Data.coord_x;
    pmsg->val_y = joy_raw_Data.coord_y;

    return JOYS_STATE_OK;
}

/*************************************************
Function: 			Joy_VersionCallback
Description:		返回摇杆模块版本号
*************************************************/
void Joy_VersionCallback(void)
{
		if (!g_bVersion)
			sdlog_printf_app("%s" ,Joy_version);
		GR_LOG_INFO("%s" ,Joy_version);
}

/*************************************************
Function: 			led_VersionCallback
Description:		返回led模块版本号
*************************************************/
void led_VersionCallback(void)
{
		if (!g_bVersion)
			sdlog_printf_app("%s" ,led_version);
		GR_LOG_INFO("%s" ,led_version);
}
