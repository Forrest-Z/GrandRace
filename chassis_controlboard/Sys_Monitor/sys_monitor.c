#include "sys_monitor.h"
#include "motor_driver.h"
#include "motor_state.h"
#include "Bms.h"
#include "Ultrasonic_New.h"
#include "imu.h"
#include "joystick.h"
#include "watchdog_task.h"
#include "sys_cpu.h"
#include "sys_stack.h"
#include <string.h>
#include "Motor_Control.h"
#include "chassis_config.h"
#include "chassis_fsm.h"
#include "SD_Log_App.h"
#include "log_app.h"
#include "SD_Driver.h"
#include "chassis_com.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

#include "alarm_com.h"
#include "temp_sensor.h"
#include "control_debug.h"

static System_State_TypeDef monitor_state;

volatile uint32_t cmdvelTimeOut = 0;


void initSystemMonitor(void)
{
    memset(&monitor_state, 0x00, sizeof(System_State_TypeDef));
}

uint8_t getSystemWorkState(void)
{
    static bool   bLedFault = false;
    if(monitor_state.comm_net != 0 &&  (getSystemTick() > 6000))
    {
        if(!bLedFault)
        {
          GR_LOG_ERROR("chassis comm_net  error, and led 3flash");
          sendLedCtl_fault();//发出故障灯
          bLedFault = true;
        }
        return SYSTEM_WORK_ANNORMAL;
        
    }
    else if((monitor_state.joystick != 0)
            || (monitor_state.motor != 0)
            || (monitor_state.motor_driver != 0)
            || (monitor_state.imu == 2))
    {
      if((getSystemTick() > 20000) && (bLedFault == false))
      {
        GR_LOG_ERROR("sys mork annormal joy=%d motor=%d driver=%d imu=%d",
        monitor_state.joystick,monitor_state.motor,
        monitor_state.motor_driver,monitor_state.imu);
        bLedFault = true;
        //电机电流日志
        printf_motor_I();
      }
    
      return SYSTEM_WORK_ANNORMAL;
    }

    bLedFault = false;
    return SYSTEM_WORK_NORMAL;
}

// 产生系统自检事件
void sendSystemCheckEvtToChassisFsm(void)
{
    if((getSystemWorkState() != SYSTEM_WORK_NORMAL) 
			&& (getChassisFsmCurState() != CHASSIS_STATE_FAULT))
    {
        sendEvtToChassisFsm(FSM_EVT_SYSTEM_FAULT);
				GR_LOG_INFO("chassis fsm fault ,because joys=%d, motor=%d, imu=%d, net= %d.", 
							monitor_state.joystick, monitor_state.motor, monitor_state.imu,monitor_state.comm_net);
     }
}

void checkUltAlarm(uint16_t ultrasonic_state,
                   uint16_t lastUlt)
{
  uint8_t bit = 0;
	char errLog[30]={0};
	char okLog[30]={0};
	char tmp[3]={0};
	
  if(lastUlt != ultrasonic_state)
  {
    for(uint8_t i = 0;i < 13;i++)
    {
        bit = GetBit(ultrasonic_state,i);//获取某一位的值
        if(bit)
        { 
          setAlarm(eA_EXTE, i+5, ALARM_ERR);
          if ( i==0 )
					{
            GR_LOG_ERROR("ult communtion error");
						return;
					}
          else
					{
						sprintf(tmp,"%d ",i);
						strcat(errLog,tmp);
					}       
        }
        else
        {
          setAlarm(eA_EXTE, i+5, ALARM_OK);
          if (i == 0)
					{
            GR_LOG_INFO("ult communtion  recover ");
					}
          else
					{
						sprintf(tmp,"%d ",i);
						strcat(okLog,tmp);
					}
        }
    }
		if(strlen(errLog) > 1)
				GR_LOG_ERROR("ult Channel %s error ",errLog);
		if(strlen(okLog) > 1)
				GR_LOG_INFO("ult Channel %s ok ",okLog);
  }
  
}

/*************************************************
Function: 			checkCmdVelAlarm
Description:		速度控制cmdvel告警监控
Input:					none
*************************************************/
//100 hz 10ms更新一次
void clearCmdVelTimeOut(void)
{
  cmdvelTimeOut = 0;
}
void checkCmdVelAlarm(void)
{
   static bool bLog = false;
  //1s超时 进入故障  
  if((cmdvelTimeOut > 100 ) && (bLog == false))
  {
      GR_LOG_ERROR("cmd_vel recv timeout,into sys fault state !!!");
      ResetRemoteControlData();
      bLog = true;
      //进入故障
      sendEvtToChassisFsm(FSM_EVT_SYSTEM_FAULT);
      setAlarm(eA_SYS,eb20_cmdvelTimeOutErr, ALARM_ERR);
  }
  else if((cmdvelTimeOut < 100) && (bLog == true))
  {
       GR_LOG_ERROR("cmd_vel recv timeout recover ");
       bLog = false;
       setAlarm(eA_SYS,eb20_cmdvelTimeOutErr, ALARM_OK);
  }
  cmdvelTimeOut++;
}

/*************************************************
Function: 			handleSysAlarmAndLog
Description:		系统告警监控日志记录
Return: 				none
*************************************************/
void handleSysAlarmAndLog(void)
{
    static uint8_t last_imu = 0,  last_flash = 0;
    static uint8_t last_joys = 0, last_wdg = 0;;
    static bool bLogStack = false, bLogCpu = false,bLedinit = false;
    static uint16_t last_motor_driver = 0,last_motor = 0;
		static uint16_t last_ultra = 0;
		static uint32_t last_bms = 0;
		
    if(getSystemTick() > 5000)
    {

#ifdef __TP_USER
        TP_STACK_OVER(eTP12_stack_over);
        TP_SYS_REBOOT(eTP0_reboot);
        //TP_MEM_LEAK(eTP13_mem_leak); //暂时无法实现
        TP_MODIFY_VALUE(eTP7_stack_sensor,monitor_state.stack.stack_sensor, (g_cfg.sA.sys.cfg.b00_sensorTaskStackErr_v-1));
        TP_MODIFY_VALUE(eTP8_stack_control,monitor_state.stack.stack_control, (g_cfg.sA.sys.cfg.b01_ctlTaskStackEerr_v-1));
        TP_MODIFY_VALUE(eTP9_stack_comm,monitor_state.stack.stack_comm, (g_cfg.sA.sys.cfg.b02_communTtaskStackErr_v-1));
				TP_MODIFY_VALUE(eTP10_stack_fsm,monitor_state.stack.stack_fsm, (g_cfg.sA.sys.cfg.b03_stateTaskStackErr_v-1));
				TP_MODIFY_VALUE(eTP11_stack_log,monitor_state.stack.stack_log, (g_cfg.sA.sys.cfg.b04_logTaskStackErr_v-1));
#endif
        if( (monitor_state.stack.stack_sensor  < g_cfg.sA.sys.cfg.b00_sensorTaskStackErr_v)
                || (monitor_state.stack.stack_control < g_cfg.sA.sys.cfg.b01_ctlTaskStackEerr_v)
                || (monitor_state.stack.stack_comm    < g_cfg.sA.sys.cfg.b02_communTtaskStackErr_v)
                || (monitor_state.stack.stack_fsm     < g_cfg.sA.sys.cfg.b03_stateTaskStackErr_v)
                || (monitor_state.stack.stack_log     < g_cfg.sA.sys.cfg.b04_logTaskStackErr_v))
        {
            if(monitor_state.stack.stack_sensor  < g_cfg.sA.sys.cfg.b00_sensorTaskStackErr_v)
            {
                setAlarm(eA_SYS, eb00_sensorTaskStackErr, ALARM_ERR);
            }
            if(monitor_state.stack.stack_control < g_cfg.sA.sys.cfg.b01_ctlTaskStackEerr_v)
            {
                setAlarm(eA_SYS, eb01_ctlTaskStackEerr, ALARM_ERR);
            }
            if(monitor_state.stack.stack_comm  < g_cfg.sA.sys.cfg.b02_communTtaskStackErr_v)
            {
                setAlarm(eA_SYS, eb02_communTtaskStackErr, ALARM_ERR);
            }
            if(monitor_state.stack.stack_fsm  < g_cfg.sA.sys.cfg.b03_stateTaskStackErr_v)
            {
                setAlarm(eA_SYS, eb03_stateTaskStackErr, ALARM_ERR);
            }
            if(monitor_state.stack.stack_log  < g_cfg.sA.sys.cfg.b04_logTaskStackErr_v)
            {
                setAlarm(eA_SYS, eb04_logTaskStackErr, ALARM_ERR);
            }

            if( !bLogStack )
            {
                GR_LOG_ERROR("system stack abnormal,stack = %d,%d,%d,%d,%d",   \
                         monitor_state.stack.stack_sensor, monitor_state.stack.stack_control,   \
                         monitor_state.stack.stack_comm, monitor_state.stack.stack_fsm, monitor_state.stack.stack_log);

                bLogStack = true;
            }
        }
				else
				{
					  if(bLogStack)
				  	{
				  		  GR_LOG_WARN("system stack recover,stack = %d,%d,%d,%d,%d",   \
                         monitor_state.stack.stack_sensor, monitor_state.stack.stack_control,   \
                         monitor_state.stack.stack_comm, monitor_state.stack.stack_fsm, monitor_state.stack.stack_log);
				  	}
						
						setAlarm(eA_SYS, eb00_sensorTaskStackErr, ALARM_OK);
						setAlarm(eA_SYS, eb01_ctlTaskStackEerr, ALARM_OK);
						setAlarm(eA_SYS, eb02_communTtaskStackErr, ALARM_OK);
						setAlarm(eA_SYS, eb03_stateTaskStackErr, ALARM_OK);
						setAlarm(eA_SYS, eb04_logTaskStackErr, ALARM_OK);
						bLogStack = false;
				}
    }

#ifdef __TP_USER
		TP_MODIFY_VALUE(eTP3_CPU_85,monitor_state.cpu_usage,(g_cfg.sA.sys.cfg.b14_cpuLoadErr_v+1));
#endif
    if(monitor_state.cpu_usage > g_cfg.sA.sys.cfg.b14_cpuLoadErr_v)
    {
        setAlarm(eA_SYS, eb14_cpuLoadErr, ALARM_ERR);
        if( !bLogCpu)
        {
            GR_LOG_ERROR("cpu error = %d", monitor_state.cpu_usage);
            bLogCpu = true;
        }
        if(monitor_state.cpu_usage >= 100)
        {
            GR_LOG_ERROR("cpu error = %d >= 100,start reboot ",monitor_state.cpu_usage);
            chassisReboot();	
        }
    }
    else
    {
        setAlarm(eA_SYS, eb14_cpuLoadErr, ALARM_OK);
				if(bLogCpu)
						GR_LOG_INFO("cpu  recover = %d", monitor_state.cpu_usage);
				
        bLogCpu = false;
    }

		  //wdg变化 记录日志
    if(last_wdg != monitor_state.wdg_state)
    {	
				if(monitor_state.wdg_state)
				{
						setAlarm(eA_SYS, eb15_wdgInfo, ALARM_ERR);
						GR_LOG_ERROR("[last][cur][%d][%d] chassis watchdog reset",last_wdg,monitor_state.wdg_state);
				}
				else
				{
						setAlarm(eA_SYS, eb15_wdgInfo, ALARM_OK);
						GR_LOG_WARN("[last][cur][%d][%d] chassis watchdog recover",last_wdg,monitor_state.wdg_state);
				}
				last_wdg = monitor_state.wdg_state;
    }

    // 开机5s内不记录下面的故障(系统稳定后记录)
    if(getSystemTick() < 6000) 
    {
       if(getSystemTick() > 5000 && bLedinit == false)	
       {
          //确保执行一次
          bLedinit = true;
          sendLedCtl_ChassisInit();
          //更新获取版本号
          Log_version();
       }
       return;
    }
		
    //监控cmd_vel 超时处理
    checkCmdVelAlarm();
		
    //bms变化 记录日志
    if(last_bms != monitor_state.bms)
    {
				last_bms = monitor_state.bms;
        GR_LOG_ERROR("bms abnormal,[%d]bms = %d",last_bms, monitor_state.bms);
    }
    

    // motor 变化 记录日志
    if(last_motor != monitor_state.motor)
    {
        GR_LOG_ERROR("motor event,[%d]motor= %d",last_motor, monitor_state.motor);
				last_motor = monitor_state.motor;
    }

    // motor_driver 变化 记录日志
    if(last_motor_driver != monitor_state.motor_driver)
    {
        GR_LOG_ERROR("motor driver event,[%d]motor_driver = %d",last_motor_driver, monitor_state.motor_driver);
				last_motor_driver = monitor_state.motor_driver;
    }
    

    //摇杆变化 记录日志
    if(last_joys != monitor_state.joystick)
    {
        GR_LOG_ERROR("joystick abnormal,[%d]joystick = %d",last_joys, monitor_state.joystick);
				last_joys = monitor_state.joystick;
    }
   

    //imu变化 记录日志
    if(last_imu != monitor_state.imu)
    {
        GR_LOG_ERROR("imu abnormal,[%d]imu= %d", last_imu,monitor_state.imu);
				last_imu = monitor_state.imu;
    }
 
     checkUltAlarm(monitor_state.ultrasonic, last_ultra);
    //超声波状态变化 记录日志
    if(last_ultra != monitor_state.ultrasonic)
    {
        GR_LOG_ERROR("ultrasonic abnormal,[%d]ultrasonic = %d",last_ultra, monitor_state.ultrasonic);
				last_ultra = monitor_state.ultrasonic;
    }
    
		 //sd日志存满记录日志
    if(last_flash != monitor_state.flash)
    {
        GR_LOG_ERROR("flash abnormal,[%d]flash = %d", last_flash,monitor_state.flash);
				last_flash = monitor_state.flash;
    }

}

void readSystemMonitor(void)
{
    //正常使用时
    System_Stack_Read(&monitor_state.stack);
    monitor_state.cpu_usage   = getSystemCpuPara();
    monitor_state.wdg_state   = getResetState();	
    monitor_state.bms 		    = getBmsState();
    monitor_state.motor 	    = getMotorState();
    monitor_state.motor_driver  = getMotorDriverState();
    monitor_state.joystick 	  = getJoystickState();
    monitor_state.imu 		    = getImuState();
    #ifdef __ONLINE_UPDATE_USER
    monitor_state.ultrasonic  = getUltrasonicState();
    #else 
    monitor_state.ultrasonic = 0;
    #endif
    monitor_state.weighing 	  = 0;
		monitor_state.flash       = getSdFlashState();
		monitor_state.comm_net    = getCommState();
    monitor_state.led         = getLedState();
    monitor_state.m4_temp     = getM4Temp();
    monitor_state.imu_temp    = getImuTemp();
    
    sendSystemCheckEvtToChassisFsm();
    handleSysAlarmAndLog();
}

void getSystemMonitor(System_State_TypeDef *pmsg)
{
    memcpy(pmsg, &monitor_state, sizeof(System_State_TypeDef));
}

void ClearWatchdogStateMonitor(void)
{
    ClearResetState();
}


