#include "Bms.h"
#include "chassis_config.h"
#include "sys_queue.h"
#include "chassis_com.h"
#include "SD_Log_App.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

//电池状态标志
static eBMS m_bms_state;

//记录log 表翻转标志
bBMS_LOGFLAG bBmsLog = {0};

/*************************************************
Function: 			handleBmsChargeEvent
Description:		检查发送是否充电事件
Input:					charge_flag  : 充电状态
Output: 				none
Return: 				none
*************************************************/
void handleBmsChargeEvent(unsigned char charge_flag)
{
    static unsigned char pre_charge_flag = 0xFF;

    if(getTX2CommunicationState() == 0)
    {
        return;
    }

    if(pre_charge_flag != charge_flag)
    {
        if(charge_flag == 1)
        {
            pushChassisEvt(CHASSIS_CHARGING_START_EVT);
        }
        else
        {
            pushChassisEvt(CHASSIS_CHARGING_END_EVT);
        }

        pre_charge_flag = charge_flag;
    }
}

/*************************************************
Function: 			checkBmsData
Description:		接收电源消息 检查机制
Input:					none
Output: 				none
Return: 				none
*************************************************/
void checkBmsData(void)
{
    int32_t temperature;
    int bmsCommStatus = 0,bmsWarn = 0;
    eBMS bms;

    bms.u32_v = ALARM_OK;

    //充电事件处理
#ifdef __BMD_CHARG_EN 
    handleBmsChargeEvent(Bms_Info.status);
#else
    Bms_Info.status = 0;
#endif

    bmsCommStatus = getBMSCommunicationState();
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP56_main2powerCommun_err,bmsCommStatus, 0);
#endif
    if (bmsCommStatus == 0)
    {
				if(bBmsLog.bPowerComFlag < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bPowerComFlag++;
				}
        else if (bBmsLog.bPowerComFlag == BMS_LOG_MAX_CNT)
        {
          bms.flag.b05_dis_connection = ALARM_ERR;
          //清楚所有的告警
          clearAlarm(eA_POWER);
          //只设置电源断开的告警即可
          setAlarm(eA_POWER,eb05_main2powerCommun_err,ALARM_ERR);
          GR_LOG_ERROR("power BMSCommunication error!");
          bBmsLog.bPowerComFlag = true;
          m_bms_state.u32_v = bms.u32_v;
          bBmsLog.bPowerComFlag = BMS_LOG_OUT;
        }
        return;
    }
    else
    {
        if(bBmsLog.bPowerComFlag == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power BMSCommunication ok!");
        }
        setAlarm(eA_POWER,eb05_main2powerCommun_err,ALARM_OK);
        bBmsLog.bPowerComFlag = 0;
        bms.flag.b05_dis_connection = ALARM_OK;
    }

    /***************Volatage_5V*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP27_5v_error,Bms_Info.Volatage_5V_Error_Flag, 1);
#endif
    if(Bms_Info.Volatage_5V_Error_Flag)
    {
    		if(bBmsLog.bV_5vErrorFlag < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bV_5vErrorFlag++;
				}
        else if (bBmsLog.bV_5vErrorFlag == BMS_LOG_MAX_CNT)
        {
          GR_LOG_ERROR("power volatage 5V error!");
          setAlarm(eA_POWER,eb14_5v_V_warn,ALARM_ERR);
          bms.flag.b14_5v_power_v = ALARM_ERR;
          bBmsLog.bV_5vErrorFlag = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bV_5vErrorFlag == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power volatage 5V error recover");
        }
        bBmsLog.bV_5vErrorFlag = 0;
        setAlarm(eA_POWER,eb14_5v_V_warn,ALARM_OK);
        bms.flag.b14_5v_power_v = ALARM_OK;
    }

    /***************Volatage_In12V*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP28_in12v_error,Bms_Info.Volatage_In12V_Error_Flag, 1);
#endif
    if(Bms_Info.Volatage_In12V_Error_Flag)
    {
        if(bBmsLog.bV_In12vErrFlag < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bV_In12vErrFlag++;
				}
        else if (bBmsLog.bV_In12vErrFlag == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb12_12v_inside_V_warn,ALARM_ERR);
          bms.flag.b12_12v_int_power_v = ALARM_ERR;
          GR_LOG_ERROR("power volatage In12V error!");
          bBmsLog.bV_In12vErrFlag  = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bV_In12vErrFlag == BMS_LOG_OUT)
        {
          GR_LOG_INFO("power volatage In12V recover ");
        }
        bBmsLog.bV_In12vErrFlag = 0;
        setAlarm(eA_POWER,eb12_12v_inside_V_warn,ALARM_OK);
        bms.flag.b12_12v_int_power_v = ALARM_OK;
    }

    /***************Volatage_Out12V*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP25_out12v_error,Bms_Info.Volatage_Out12V_Error_Flag, 1);
#endif
    if(Bms_Info.Volatage_Out12V_Error_Flag)
    {
        if(bBmsLog.bV_Out12vErrFlag < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bV_Out12vErrFlag++;
				}
        else if (bBmsLog.bV_Out12vErrFlag == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb15_12v_out_V_warn,ALARM_ERR);
          bms.flag.b15_12v_out_power_v = ALARM_ERR;
          GR_LOG_ERROR("power volatage Out12V error!");
          bBmsLog.bV_Out12vErrFlag = BMS_LOG_OUT;
        } 
    }
    else
    {
        if(bBmsLog.bV_Out12vErrFlag == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power volatage Out12V recover!");
        }
        bBmsLog.bV_Out12vErrFlag = 0;
        setAlarm(eA_POWER,eb15_12v_out_V_warn,ALARM_OK);
        bms.flag.b15_12v_out_power_v = ALARM_OK;
    }
    /***************Volatage_TX2*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP26_tx2_volotage_error,Bms_Info.Volatage_TX2_Error_Flag, 1);
#endif
    if(Bms_Info.Volatage_TX2_Error_Flag)
    {
        if(bBmsLog.bV_Tx2ErrFlag < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bV_Tx2ErrFlag++;
				}
        else if (bBmsLog.bV_Tx2ErrFlag == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb13_tx2_V_warn,ALARM_ERR);
          bms.flag.b13_tx2_power_v = ALARM_ERR;
          GR_LOG_ERROR("power volatage TX2 error!");
          bBmsLog.bV_Tx2ErrFlag = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bV_Tx2ErrFlag == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power volatage TX2 recover!");
        }
        bBmsLog.bV_Tx2ErrFlag = 0;
        setAlarm(eA_POWER,eb13_tx2_V_warn,ALARM_OK);
        bms.flag.b13_tx2_power_v = ALARM_OK;
    }

    /***************JOY_CURRENT*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP31_joy_current_error,Bms_Info.JOY_CURRENT_STATUS, 1);
#endif
    if(Bms_Info.JOY_CURRENT_STATUS)
    {
        if(bBmsLog.bJoyCurrentStatus < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bJoyCurrentStatus++;
				}
        else if (bBmsLog.bJoyCurrentStatus == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb22_joys_overI_fatal,ALARM_ERR);
          bms.flag.b18_joys_over_i = ALARM_ERR;
          GR_LOG_ERROR("power joy over current!");
          bBmsLog.bJoyCurrentStatus = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bJoyCurrentStatus == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power joy over current recover!");
        }
        bBmsLog.bJoyCurrentStatus = 0;
        setAlarm(eA_POWER,eb22_joys_overI_fatal,ALARM_OK);
        bms.flag.b18_joys_over_i = ALARM_OK;
    }

    /***************PAD_CURRENT*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP29_pad_current_error,Bms_Info.PAD_CURRENT_STATUS, 1);
#endif
    if(Bms_Info.PAD_CURRENT_STATUS)
    {
        if(bBmsLog.bPadCurStatus < BMS_LOG_MAX_CNT)
				{
          bBmsLog.bPadCurStatus++;
				}
        else if (bBmsLog.bPadCurStatus == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb20_pad_overI_fatal,ALARM_ERR);
          bms.flag.b16_pad_power_i = ALARM_ERR;
          GR_LOG_ERROR("power pad over current!");
          bBmsLog.bPadCurStatus = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bPadCurStatus == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power pad over current recover!");
        }
        bBmsLog.bPadCurStatus = 0;
        setAlarm(eA_POWER,eb20_pad_overI_fatal,ALARM_OK);
        bms.flag.b16_pad_power_i = ALARM_OK;
    }

    /********外设供电过流(超声波/路由器/RTK)*******/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP30_rtk_current_error,Bms_Info.RTK_CURRENT_STATUS, 1);
#endif
    if(Bms_Info.RTK_CURRENT_STATUS)
    {
        if(bBmsLog.bRtkCurSatus < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bRtkCurSatus++;
        }
        else if (bBmsLog.bRtkCurSatus == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb21_out_overI_fatal,ALARM_ERR);
          bms.flag.b17_out_i = ALARM_ERR;
          GR_LOG_ERROR("power rtk over current!");
          bBmsLog.bRtkCurSatus = BMS_LOG_OUT;
        }
    }
    else
    {
        if(bBmsLog.bRtkCurSatus == BMS_LOG_OUT)
        {
            GR_LOG_INFO("power rtk over current recover!");
        }
        bBmsLog.bRtkCurSatus = 0;
        setAlarm(eA_POWER,eb21_out_overI_fatal,ALARM_OK);
        bms.flag.b17_out_i = ALARM_OK;
    }
//===============================================================================================================================//
//	if(Bms_Info.warning == 7)
    bmsWarn = Bms_Info.warning & 0x40;
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP57_power2bmsCommun_err,bmsWarn, 1);
#endif
    if (bmsWarn != 0)
    {
        if(bBmsLog.bBmsDisc < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsDisc++;
        }
        else if (bBmsLog.bBmsDisc == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb06_power2bmsCommun_err,ALARM_ERR);
          bms.flag.b06_get_data_faild = ALARM_ERR;
          m_bms_state.u32_v = bms.u32_v;
          GR_LOG_WARN("power bms disc comm");
          bBmsLog.bBmsDisc = BMS_LOG_OUT;
        }
        return;
    }
    else
    {
        if(bBmsLog.bBmsDisc == BMS_LOG_OUT)
        {
            GR_LOG_WARN("power bms disc comm revoer !");
        }
        setAlarm(eA_POWER,eb06_power2bmsCommun_err,ALARM_OK);
        bms.flag.b06_get_data_faild = ALARM_OK;
        bBmsLog.bBmsDisc = 0;
    }

    temperature = ((Bms_Info.temperature1 > Bms_Info.temperature2) ? Bms_Info.temperature1 : Bms_Info.temperature2);

#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP5_bms_ultra_low_temp,temperature, (WARN_TH_TEMPER_ULTRALOW-1));
    TP_MODIFY_VALUE(eTP18_bms_low_temp,temperature, (WARN_TH_TEMPER_LOW-1));
    TP_MODIFY_VALUE(eTP6_bms_ultra_high_temp,temperature,(WARN_TH_TEMPER_ULTRAHIGH+1));
    TP_MODIFY_VALUE(eTP19_bms_high_temp,temperature, (WARN_TH_TEMPER_HIGH+1));
#endif
    /************temperature****************/
    if(temperature < WARN_TH_TEMPER_ULTRALOW)
    {
        if(bBmsLog.bBmsTemp < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsTemp++;
        }
        else if (bBmsLog.bBmsTemp == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb08_ultraLow_Temp_err,ALARM_ERR);
          bms.flag.b01_low_temp = ALARM_ERR;
          bms.flag.b08_ultra_low_temp = ALARM_ERR;
          GR_LOG_ERROR("bms temperature=[%d] ULTRALOW!",temperature);
          bBmsLog.bBmsTemp = BMS_LOG_OUT;
        }
        
    }
    else if(temperature < WARN_TH_TEMPER_LOW)
    {
        if(bBmsLog.bBmsTemp < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsTemp++;
        }
        else if (bBmsLog.bBmsTemp == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb01_bmsTempLow_warn,ALARM_ERR);
          GR_LOG_WARN("bms temperature LOW=[%d]",temperature);
          bBmsLog.bBmsTemp = BMS_LOG_OUT;
          //低温不影响手自动驾驶
          // bms.flag.b01_low_temp = ALARM_ERR;
        }
    }
    else if(temperature > WARN_TH_TEMPER_ULTRAHIGH)
    {
        if(bBmsLog.bBmsTemp < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsTemp++;
        }
        else if (bBmsLog.bBmsTemp == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb09_ultraHight_Temp_err,ALARM_ERR);
          bms.flag.b02_high_temp = ALARM_ERR;
          bms.flag.b09_ultra_high_temp = ALARM_ERR;
          GR_LOG_ERROR("bms temperature ULTRAHIGH=[%d]",temperature);
          bBmsLog.bBmsTemp = BMS_LOG_OUT;
        }
    }
    else if(temperature > WARN_TH_TEMPER_HIGH)
    {
        if(bBmsLog.bBmsTemp < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsTemp++;
        }
        else if (bBmsLog.bBmsTemp == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb02_bmsTempHigt_warn,ALARM_ERR);
          GR_LOG_WARN("bms temperature HIGH=[%d]",temperature);
          bBmsLog.bBmsTemp = BMS_LOG_OUT;
          //高温不影响手自动驾驶
          //bms.flag.b02_high_temp = ALARM_ERR;
        }
    }
    else
    {
        if(bBmsLog.bBmsTemp == BMS_LOG_OUT)
        {
            GR_LOG_WARN("bms temperature =[%d] ",temperature);
        }
        bBmsLog.bBmsTemp = 0;
        setAlarm(eA_POWER,eb08_ultraLow_Temp_err,ALARM_OK);
        setAlarm(eA_POWER,eb01_bmsTempLow_warn,ALARM_OK);
        setAlarm(eA_POWER,eb09_ultraHight_Temp_err,ALARM_OK);
        setAlarm(eA_POWER,eb02_bmsTempHigt_warn,ALARM_OK);
        bms.flag.b01_low_temp = ALARM_OK;
        bms.flag.b08_ultra_low_temp = ALARM_OK;
        bms.flag.b02_high_temp = ALARM_OK;
        bms.flag.b09_ultra_high_temp = ALARM_OK;
    }

    /***************BMS CURRENT*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP20_high_current,Bms_Info.current, (WARN_TH_CURRENT_HIGH+1));
    TP_MODIFY_VALUE(eTP23_ultrahigh_current,Bms_Info.current, (WARN_TH_CURRENT_EXCESS+1));
#endif
    if(Bms_Info.current > WARN_TH_CURRENT_EXCESS)
    {
        if(bBmsLog.bBmsI < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsI++;
        }
        else if (bBmsLog.bBmsI == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb10_ultraHight_I_err,ALARM_ERR);
          bms.flag.b03_high_current = ALARM_ERR;
          bms.flag.b10_ultra_high_current = ALARM_ERR;
          GR_LOG_ERROR("bms over very current[%f]",Bms_Info.current);
          bBmsLog.bBmsI = BMS_LOG_OUT;
        }
    }
    else if(Bms_Info.current > WARN_TH_CURRENT_HIGH)
    {
        if(bBmsLog.bBmsI < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsI++;
        }
        else if (bBmsLog.bBmsI == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb03_powerHightI_warn,ALARM_ERR);
          GR_LOG_ERROR("bms over current[%f]",Bms_Info.current);
          bBmsLog.bBmsI = BMS_LOG_OUT;
          //高电流不影响手自动驾驶
          //bms.flag.b03_high_current = ALARM_ERR;
        }
    }
    else
    {
        if(bBmsLog.bBmsI == BMS_LOG_OUT)
        {
            GR_LOG_ERROR("bms over current=[%f]",Bms_Info.current);
        }
        setAlarm(eA_POWER,eb10_ultraHight_I_err,ALARM_OK);
        setAlarm(eA_POWER,eb03_powerHightI_warn,ALARM_OK);
        bms.flag.b03_high_current = ALARM_OK;
        bms.flag.b10_ultra_high_current = ALARM_OK;
        bBmsLog.bBmsI = 0;
    }

    /***************BMS VOLATAGE*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP21_low_volotage,Bms_Info.total_volatage, (WARN_TH_VOLTAGE_LOW-1));
    TP_MODIFY_VALUE(eTP24_ultralow_volotage,Bms_Info.total_volatage, (WARN_TH_VOLTAGE_ULTRALOW-1));
#endif
    if(Bms_Info.total_volatage < WARN_TH_VOLTAGE_ULTRALOW)
    {
        if(bBmsLog.bBmsV < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsV++;
        }
        else if (bBmsLog.bBmsV == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb11_ultraHight_V_err,ALARM_ERR);
          bms.flag.b04_low_voltage = ALARM_ERR;
          bms.flag.b11_ultra_low_voltage = ALARM_ERR;
          GR_LOG_ERROR("bms volatage ultralow=[%d]",Bms_Info.total_volatage);
          bBmsLog.bBmsV = BMS_LOG_OUT;
        }
    }
    else if(Bms_Info.total_volatage < WARN_TH_VOLTAGE_LOW)
    {
        if(bBmsLog.bBmsV < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsV++;
        }
        else if (bBmsLog.bBmsV == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb04_powerLowV_warn,ALARM_ERR);
          GR_LOG_WARN("bms volatage low=[%d]",Bms_Info.total_volatage);
          bBmsLog.bBmsV = BMS_LOG_OUT;
          //低电压不影响手自动驾驶
          //bms.flag.b04_low_voltage = ALARM_ERR;
        }
    }
    else
    {
        if( bBmsLog.bBmsV == BMS_LOG_OUT)
        {
            GR_LOG_WARN("bms volatage =[%d]",Bms_Info.total_volatage);
        }
        bBmsLog.bBmsV = 0;
        setAlarm(eA_POWER,eb11_ultraHight_V_err,ALARM_OK);
        setAlarm(eA_POWER,eb04_powerLowV_warn,ALARM_OK);
        bms.flag.b04_low_voltage = ALARM_OK;
        bms.flag.b11_ultra_low_voltage = ALARM_OK;
    }

    /***************BMS SOC*****************/
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP17_bms_low_cap,Bms_Info.RelativeStateOfCharge, WARN_TH_CAPACITY_LOW-1);
    TP_MODIFY_VALUE(eTP22_bms_ultralow_cap,Bms_Info.RelativeStateOfCharge, WARN_TH_CAPACITY_ULTRALOW-1);
#endif
    if(Bms_Info.RelativeStateOfCharge < WARN_TH_CAPACITY_ULTRALOW)
    {
        if(bBmsLog.bBmsSoc < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsSoc++;
        }
        else if (bBmsLog.bBmsSoc == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb07_ultraLow_PowerCapa_err,ALARM_ERR);
          bms.flag.b00_low_power = ALARM_ERR;
          bms.flag.b07_ultra_low_power = ALARM_ERR;
          GR_LOG_ERROR("bms soc cap ultrelow=[%d] ",Bms_Info.RelativeStateOfCharge);
          bBmsLog.bBmsSoc = BMS_LOG_OUT;
        }
    }
    else if(Bms_Info.RelativeStateOfCharge < WARN_TH_CAPACITY_LOW)
    {
        if(bBmsLog.bBmsSoc < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bBmsSoc++;
        }
        else if (bBmsLog.bBmsSoc == BMS_LOG_MAX_CNT)
        {
          setAlarm(eA_POWER,eb00_bmsPowerLow_warn,ALARM_ERR);
          GR_LOG_WARN("bms soc cap low =[%d]",Bms_Info.RelativeStateOfCharge);
          bBmsLog.bBmsSoc = BMS_LOG_OUT;
          
          //bms低电量不影响手自动驾驶
          //bms.flag.b00_low_power = ALARM_ERR;
        }
    }
    else
    {
        if(bBmsLog.bBmsSoc == BMS_LOG_OUT)
        {
            GR_LOG_WARN("bms soc cap low|ultanlow recover =[%d]",Bms_Info.RelativeStateOfCharge);
        }
        setAlarm(eA_POWER,eb07_ultraLow_PowerCapa_err,ALARM_OK);
        setAlarm(eA_POWER,eb00_bmsPowerLow_warn,ALARM_OK);
        bms.flag.b00_low_power = ALARM_OK;
        bms.flag.b07_ultra_low_power = ALARM_OK;
        bBmsLog.bBmsSoc = 0;
    }

#ifdef __TP_USER
		TP_MODIFY_VALUE(eTP64_powerTempWarn,Bms_Info.powerboard_temp,(g_cfg.sA.power.cfg.b23_powerTempWarn_v+1));
		TP_MODIFY_VALUE(eTP65_powerTempErr,Bms_Info.powerboard_temp,(g_cfg.sA.power.cfg.b24_powerTempErr_v+1));
#endif
    //Powerboard Temp
    if(Bms_Info.powerboard_temp > g_cfg.sA.power.cfg.b24_powerTempErr_v)
    {
        if(bBmsLog.bPowerT < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bPowerT++;
        }
        else if (bBmsLog.bPowerT == BMS_LOG_MAX_CNT)
        {
          GR_LOG_ERROR("powerT emp hight Err=[%f] ",Bms_Info.powerboard_temp);
          setAlarm(eA_POWER,eb24_powerTempErr,ALARM_ERR);
          bBmsLog.bPowerT = BMS_LOG_OUT;
        }
    }
		else if(Bms_Info.powerboard_temp > g_cfg.sA.power.cfg.b23_powerTempWarn_v)
    {
        if(bBmsLog.bPowerT < BMS_LOG_MAX_CNT)
        {
          bBmsLog.bPowerT++;
        }
        else if (bBmsLog.bPowerT == BMS_LOG_MAX_CNT)
        {
          GR_LOG_ERROR("power Temp  wran=[%f] ",Bms_Info.powerboard_temp);
          setAlarm(eA_POWER,eb23_powerTempWarn,ALARM_ERR);
          bBmsLog.bPowerT = BMS_LOG_OUT;
        }
		}
		else
		{
			if(bBmsLog.bPowerT == BMS_LOG_OUT)
			{
					GR_LOG_WARN("power Temp recover=[%f]",Bms_Info.powerboard_temp);
			}
			setAlarm(eA_POWER,eb23_powerTempWarn,ALARM_OK);
			setAlarm(eA_POWER,eb24_powerTempErr,ALARM_OK);
      bBmsLog.bPowerT = 0;
		}

    m_bms_state.u32_v = bms.u32_v;
}

uint32_t getBmsState(void)
{
    if(getBMSCommunicationState() == 0)
    {
        //bms断连 (0x01 >> 5 ) =32
        return BMS_DISCONNECT;
    }
    return m_bms_state.u32_v;
}

void clearBmsState(void)
{
    m_bms_state.u32_v = ALARM_OK;
}
float getBmsTotalVolt(void)
{
	return Bms_Info.total_volatage;
}

