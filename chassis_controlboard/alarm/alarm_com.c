/**************************************************************************
 * @file     alarm_com.c
 * @brief    系统所有告警接口。定义处理下位机所有告警
 * @version  V2.0
 * @date     2019-06-08
 * @author   ycy
 ******************************************************************************/
#include "alarm_com.h"
#include "chassis_com.h"
#include <stdio.h>
#include <string.h>
#include "SD_Log_Driver.h"
#include "control_debug.h"

#ifdef __ALARM_USER
unPUB_ALARM g_sAlarmPub = {0};
unPUB_ALARM g_sAlarm = {0};
unCHASSIS_ALARM_cfg g_cfg;
#else
int g_sAlarmPub = 0;
#endif

/*************************************************
Function: 			setAlarm
Description:		设置模块告警
Input:					_mod  : 告警模块
								_bit  : 告警名称
								_flag : 0=恢复告警 1=设置告警
Output: 				none
Return: 				true false
*************************************************/
void setAlarm(eALARM_MOD _mod,uint8_t _bit,uint8_t _flag)
{
#ifdef __ALARM_USER
    switch (_mod)
    {
    case eA_SYS:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.sys.u64_v, _bit,_flag);
        break;
    }
    case eA_MOTOR:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.motor.u64_v, _bit,_flag);
        break;
    }
    case eA_DRIVER:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.driver.u64_v, _bit,_flag);
        break;
    }
    case eA_EXTE:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.exte.u64_v, _bit,_flag);
        break;
    }
    case eA_INTE:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.inte.u32_v, _bit,_flag);
        break;
    }
    case eA_POWER:
    {
        WriteBit(g_sAlarmPub.sA.unA.alarm.power.u32_v, _bit,_flag);
        break;
    }
    default:
    {
        //log
        break;
    }
    }
#endif

}

/*************************************************
Function: 			getAlarm
Description:		获取模块告警
Input:					_mod  : 告警模块
								_bit  : 告警名称
Output: 				none
Return: 				flag_ ： 0 | 1
*************************************************/
uint8_t getAlarm(eALARM_MOD _mod,uint8_t _bit)
{
#ifdef __ALARM_USER
    uint8_t  flag_ = 0;
    switch (_mod)
    {
    case eA_SYS:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.sys.u64_v, _bit);
        break;
    }
    case eA_MOTOR:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.motor.u64_v, _bit);
        break;
    }
    case eA_DRIVER:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.driver.u64_v, _bit);
        break;
    }
    case eA_EXTE:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.exte.u64_v, _bit);
        break;
    }
    case eA_INTE:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.inte.u32_v, _bit);
        break;
    }
    case eA_POWER:
    {
        flag_ = GetBit(g_sAlarmPub.sA.unA.alarm.power.u32_v, _bit);
        break;
    }
    default:
    {
        //log
        flag_ = ALARM_ERR;
        break;
    }
    }
    return flag_;
#else
    return 1;
#endif
}

/*************************************************
Function: 			clearAlarm
Description:		清除模块告警，即设置模块无告警
Input:					_mod  : 告警模块
Output: 				none
Return: 				none
*************************************************/
void clearAlarm(eALARM_MOD _mod)
{
#ifdef __ALARM_USER
    switch (_mod)
    {
    case eA_SYS:
    {
        g_sAlarmPub.sA.unA.alarm.sys.u64_v = ALARM_OK;
        break;
    }
    case eA_MOTOR:
    {
        g_sAlarmPub.sA.unA.alarm.motor.u64_v = ALARM_OK;
        break;
    }
    case eA_DRIVER:
    {
        g_sAlarmPub.sA.unA.alarm.driver.u64_v = ALARM_OK;
        break;
    }
    case eA_EXTE:
    {
        g_sAlarmPub.sA.unA.alarm.exte.u64_v = ALARM_OK;
        break;
    }
    case eA_INTE:
    {
        g_sAlarmPub.sA.unA.alarm.inte.u32_v = ALARM_OK;
        break;
    }
    case eA_POWER:
    {
        g_sAlarmPub.sA.unA.alarm.power.u32_v = ALARM_OK;
        break;
    }
    default:
    {
        //log
        break;
    }
    }

#endif

}

/*************************************************
Function:    initAlarm
Description:  清除所有模块告警, 设置告警阈值
Input:     none
Output:     none
Return:     none
*************************************************/
void initAlarm(void)
{
#ifdef __ALARM_USER
    uint8_t i;
    for(i=0; i<MOD_NUM; i++)
    {
        clearAlarm((eALARM_MOD)i);
    }

    g_cfg = (unCHASSIS_ALARM_cfg)
    {
        .sA.sys.cfg=
        {
            .b00_sensorTaskStackErr_v = 50,
            .b01_ctlTaskStackEerr_v = 50,
            .b02_communTtaskStackErr_v = 50,
            .b03_stateTaskStackErr_v = 50,
            .b04_logTaskStackErr_v = 50,
            .b05_cmdTaskStackErr_v =50,

            .b14_cpuLoadErr_v = 84,
            .b17_cpuTempWarn_v = 90,
            .b18_cpuTempErr_v = 95,
            .b19 = 0
        },
        .sA.power.cfg =
        {
            .b01_bmsTempLow_warn_v = -10,
            .b02_bmsTempHigt_warn_v = 60,

            .b08_ultraLow_Temp_err_v = -20,
            .b09_ultraHight_Temp_err_v = 70,

            .b23_powerTempWarn_v = 95,
            .b24_powerTempErr_v = 100
        },
        .sA.inte.cfg =
        {
            .b04_imuTempErr_v = 90,
            .reserve = 0
        }
    };

#ifdef __ONLINE_UPDATE_USER
  //系统支持升级 无告警
  setAlarm(eA_SYS,eb16_updateSupportErr,ALARM_OK);
#else
  //系统不支持升级 则告警
 // setAlarm(eA_SYS,eb16_updateSupportErr,ALARM_ERR);
#endif


#endif
}
/*************************************************
Function: 			alarmMonitor
Description:		记录告警日志 有变化的时候需要写
Input:					none
Output: 				isChange:是否有变化
Return: 				bool 是否有变化
*************************************************/
bool alarmMonitor(void)
{
#ifdef __ALARM_USER
    if((g_sAlarm.sA.unA.alarm.sys.u64_v   != g_sAlarmPub.sA.unA.alarm.sys.u64_v)    ||
            (g_sAlarm.sA.unA.alarm.motor.u64_v  != g_sAlarmPub.sA.unA.alarm.motor.u64_v)  ||
            (g_sAlarm.sA.unA.alarm.driver.u64_v != g_sAlarmPub.sA.unA.alarm.driver.u64_v) ||
            (g_sAlarm.sA.unA.alarm.exte.u64_v   != g_sAlarmPub.sA.unA.alarm.exte.u64_v)   ||
            (g_sAlarm.sA.unA.alarm.inte.u32_v   != g_sAlarmPub.sA.unA.alarm.inte.u32_v)   ||
            (g_sAlarm.sA.unA.alarm.power.u32_v  != g_sAlarmPub.sA.unA.alarm.power.u32_v) )
    {
        memcpy(g_sAlarm.buff, g_sAlarmPub.buff, sizeof(unPUB_ALARM));
        GR_LOG_ERROR("alarm sys=%llu motor=%llu driver=%llu exte=%llu inte= %d power=%d"
					,g_sAlarm.sA.unA.alarm.sys.u64_v		,g_sAlarm.sA.unA.alarm.motor.u64_v
					,g_sAlarm.sA.unA.alarm.driver.u64_v	,g_sAlarm.sA.unA.alarm.exte.u64_v
					,g_sAlarm.sA.unA.alarm.inte.u32_v	,g_sAlarm.sA.unA.alarm.power.u32_v);

          //电机电流日志
          printf_motor_I();
					
			return true;
    }

		return false;
#endif
}
