#include "log_app.h"
#include "log_cmd.h"
#include "Clock.h"
#include <string.h>
#include <stdlib.h>
#include "watchdog_task.h"
#include "chassis_config.h"
#include "chassis_com.h"
#include "joystick.h"
#include "Subscribe_Pc.h"
#include "Motor_Control.h"
#include "imu_maintain.h"
#include "fault_injector_wrapper.h"
#include "Motor_PcConfig.h"
#include "SD_Log_App.h"
#include "chassis_com.h"
#include "Publish_powerboard.h"
#include "Publish_outboard.h"
#include "FTP_APP.h"
#include "Communication.h"
#include "buffer.h"


char file_name[8];
static stLog_sd log_sd_cmd;
volatile bool g_bVersion = 0;


void log_sd_cmd_init(void);


bool Log_syscall_lookup(const char* name, uint8_t* num)
{
    uint8_t index;

    for (index = 0; index < LOG_COMMAND_NUM; index++)
    {
        if (strncmp(_syscall_table[index].cmd, name, 
              strlen(_syscall_table[index].cmd)) == 0 )
        {
            *num = index;
            return true;
        }
    }
    return false;
}

void Log_help(void)
{
    uint8_t index;

    sdlog_printf_app("%s", "System shell commands:");
    for (index = 0; index < LOG_COMMAND_NUM; index++)
    {
        SDLog_info_port("%s", _syscall_table[index].name);
        SDLog_info_port("%s", _syscall_table[index].desc);
        SDLog_info_push();
        delayMs(4);
    }
}

void Log_main_version(void)
{
    sdlog_printf_app("[%s %s %s];", __DATE__, __TIME__,MAIN_VERSION);
    printfChassisVersion();
}
void Log_power_version(void)
{
    g_bVersion = false;
    publishFrameData(POW_ID,PowerVer_ID,&getPowerVer,sizeof(getPowerVer));
}
void Log_joy_version(void)
{
    g_bVersion = false;
    publishFrameData(JOYS_ID,JoyVer_ID,&getJoyVer,sizeof(getJoyVer));
}
void Log_ult_version(void)
{
    g_bVersion = false;
    publishFrameData(ULT_ID,PUB_UltVer_ID,&getUltVer,sizeof(getUltVer));
}
void Log_led_version(void)
{
    g_bVersion = false;
    publishFrameData(LED_ID,PUB_LED_VER_ID, &get_ledversin,sizeof(get_ledversin));
}

void Log_version(void)
{
    g_bVersion = true;

    publishFrameData(POW_ID,PowerVer_ID,&getPowerVer,sizeof(getPowerVer));
    publishFrameData(JOYS_ID,JoyVer_ID,&getJoyVer,sizeof(getJoyVer));
    publishFrameData(ULT_ID,PUB_UltVer_ID,&getUltVer,sizeof(getUltVer));
    publishFrameData(LED_ID,PUB_LED_VER_ID, &get_ledversin,sizeof(get_ledversin));
    sdlog_printf_app("[%s %s %s] %s %s %s %s;",__DATE__, __TIME__
                     , MAIN_VERSION,Power_version,Joy_version,ult_version,led_version);
    GR_LOG_INFO("[%s %s %s] %s %s %s %s;",__DATE__, __TIME__
                , MAIN_VERSION,Power_version,Joy_version,ult_version,led_version);
}

void log_sd_cmd_init(void)
{
    memset(&log_sd_cmd, 0x00, sizeof(stLog_sd));
}

void App_log_cmd(void)
{
    static uint32_t t = 0;
    static uint8_t log_status = LOG_RW_ERROR;

    if(log_sd_cmd.cmd == LOG_CMD_READ)
    {
        getlogday();//获取当前时间
        log_status = read_log_app(logtime.LOG_DAY);
        if(log_status == LOG_RW_OK)
        {
            sdlog_printf_app("%s", "Read ok!;");
            GR_LOG_INFO("%s", "log sd read ok .;")
            log_sd_cmd.cmd = 0;
        }
        else if(log_status == LOG_SYSTEM_BUSY)
        {
            sdlog_printf_app("%s", "Log Systme Busy Now!;");
            log_sd_cmd.cmd = 0;
        }
        else if(log_status == LOG_RW_ERROR)
        {
            //FATFS ERROR! link=0, init=0
            sdlog_printf_app("FATFS ERROR! link=%d, init=%d", FAT_LIKINT, FAT_FMOINT);
            log_sd_cmd.cmd = 0;
        }

    }
    else if(log_sd_cmd.cmd == LOG_CMD_CLEAN)
    {
        if(!getParkingState())
        {
            sdlog_printf_app("Error, in motion, Please parking and clean;");
            GR_LOG_ERROR("Error, in motion, Please parking and clean;");
            return;
        }
        Watchdog_SectorErase_Reconfigure();

        getlogday();
        if(delect_file_app(logtime.LOG_DAY))
        {
            sdlog_printf_app("%s", "Clean ok!;");
            GR_LOG_INFO("%s", "log sd clean ok .;")
        }
        Watchdog_Task_Start();
        log_sd_cmd.cmd = 0;
    }
    else if(log_sd_cmd.cmd == LOG_CMD_FLUSH)
    {
        if(!getParkingState())
        {
            sdlog_printf_app("Error, in motion, Please parking and flush;");
            GR_LOG_ERROR("Error, in motion, Please parking and clean;");
            return;
        }

        sdlog_printf_app("%s", "Flush ok!;");
        log_sd_cmd.cmd = 0;
    }

    if(getSystemTick() - t >= 5000)
    {
        if(!getParkingState())
        {
            return;
        }
        t = getSystemTick();
    }
}

void Log_sd_port(const char* cmd)
{
    const char log_read[]  = "log_sd read";
    const char log_clean[] = "log_sd clean";
    const char log_flush[] = "log_sd flush";

    if (strncmp(log_read, cmd, strlen(log_read)) == 0)
    {
        for(int i = 0; i<sizeof(file_name); i++)
        {
            file_name[i] = *(strchr(cmd,':')+i);
        }
        log_sd_cmd.cmd = LOG_CMD_READ;
    }
    else if (strncmp(log_clean, cmd, strlen(log_clean)) == 0)
    {

        for(int i = 0; i<sizeof(file_name); i++)
        {
            file_name[i] = *(strchr(cmd,':')+i);
        }
        log_sd_cmd.cmd = LOG_CMD_CLEAN	;
    }
    else if (strncmp(log_flush, cmd, strlen(log_flush)) == 0)
    {
        log_sd_cmd.cmd = LOG_CMD_FLUSH;
    }
    else if(cmd[6] != ' ')
    {
        sdlog_printf_app("command not found");
        GR_LOG_ERROR("log sd  input command not found");
    }
    else
    {
        sdlog_printf_app("Please input log_sd {<read><clean><flush>}");
    }
}
/*************************************************
Function: 			log_set_level
Description:		设置日志等级 (4:dbg)|(3:info)|(2:warn)|(1:error)
Input:	cmd:命令行字符串
*************************************************/
void log_set_level(const char* cmd)
{
    uint8_t level = 0;
    if(true == token_cmd(cmd,"set_log_level",&level,true))
    {
        if(level > LOG_LEVEL_DEBUG) 	level = LOG_LEVEL_DEBUG;
        sdlog_printf_app("set log level is = %d ", level);
        GR_LOG_INFO("set log level is = %d ", level);
        setLogLevel(level);
    }
    else
    {
        sdlog_printf_app("set log level wrong ...");
        GR_LOG_ERROR("set log level wrong ...");
    }
}
/*************************************************
Function: 			log_get_level
Description:		获取日志等级 (4:dbg)|(3:info)|(2:warn)|(1:error)
Input:	none
*************************************************/
void log_get_level(void)
{
    uint8_t level = 0;
    level = getLogLevel();
    GR_LOG_INFO("get log level is = %d ", level);
    sdlog_printf_app("set log level is = %d ", level);
}

/*************************************************
Function: 			logSetEnv
Description:		设置环境参数 如set_env motor 4
Input:	cmd :set_env motor 4
*************************************************/
#define KEY_NUM 4
const char key[KEY_NUM][20] = {
    "motor",
    "driver",
    "chassis",
    "res",
};
void logSetEnv(const char* cmd)
{
    char strValue[3] = {0};
    uint8_t value = 0 ,last_value = 0;
    uint8_t i = 0,ret = 0;

    for(i=0; i<KEY_NUM; i++)
    {
        //set_env = 7
        if(true == token_cmd((char *)&cmd[7],(char *)&key[i],
                             &value,true))
        {
            if(!getParkingState())
            {
                sdlog_printf_app("Error,Please parking and config");
                return;
            }
            sprintf(strValue, "%d", value);
            ret = write_env_app((char *)&key[i],strValue);
            if(ret == LOG_RW_OK)
            {
                sdlog_printf_app("SD set %s ok,value=%s",key[i],strValue);
            }
            else if(ret == LOG_SYSTEM_BUSY)
            {
                sdlog_printf_app("SD log systme busy Now!");
            }
            else
            {
                sdlog_printf_app("SD set %s Fail", key[i]);
            }
            
            if(g_EnvCfg.sENV.type.buff[i] != value)
            {
              last_value = g_EnvCfg.sENV.type.buff[i];
              g_EnvCfg.sENV.type.buff[i]= value;
              if(set_env_flash(g_EnvCfg) == OK)
              {
                sdlog_printf_app("flash set %s ok,value = %d",key[i],value);
              }
              else
              {
                g_EnvCfg.sENV.type.buff[i]=last_value;
                sdlog_printf_app("flash set %s Fail,value = %d",key[i],value);
              }
            }
            return;
        }
    }
    sdlog_printf_app("set env Fail,input err ");
    return;
}



/*************************************************
Function:       logSetEnv_float
Description:    设置环境参数 如set_env_f joy_l 1.5
Input:  cmd :set_env_f joy_l  reset_odom xx.xx
*************************************************/
typedef enum
{
    eODOM   = 0,   //reset_odom
    eJOY_L  = 1,   //joy_l
    eJOY_A  = 2,   //joy_a
    KEY_F_NUM = 3, //KEY_F_NUM
} eENV_F;
const char key_f[KEY_F_NUM][15] = {
  "reset_odom",
  "joy_l",
  "joy_a",
};
void logSetEnv_float(const char* cmd)
{
    char strValue[5] = {0};
    float value = 0,last_value = 0;
    uint8_t i = 0,ret = 0;

    for(i=0; i<KEY_F_NUM; i++)
    {
        //set_env_f = 9
        if(true == token_cmd_f((char *)&cmd[9],(char *)&key_f[i],&value,true))
        {
           if( ( i == eJOY_L) && (value > JOY_MAX_V))
           {
               sdlog_printf_app("set %s Fail value[%f] > %f", key_f[i],value,JOY_MAX_V);
               return;
           } 
           else  if( ( i == eJOY_A) && (value > JOY_MAX_A))
           {
             sdlog_printf_app("set %s Fail value[%f] > %f", key_f[i],value,JOY_MAX_A);
             return;
           }
						
            //标定 odom 编码
            if (i == eODOM)
            {
              //配置0为默认值 其他为设置值
							value = reset_odom_encoder(value);
            }
       
					 //储存
            sprintf(strValue, "%f", value);
            ret = write_env_app((char *)&key_f[i],strValue);
            if(ret == LOG_RW_OK)
            {
                sdlog_printf_app("SD set %s ok, value = %s",key_f[i],strValue);
            }
            else if(ret == LOG_SYSTEM_BUSY)
            {
                sdlog_printf_app("SD systme busy Now!;");
            }
            else
            {
                sdlog_printf_app("SD set %s Fail", key_f[i]);
            }

            if(g_EnvCfg.sENV.f.buff[i] != value)
            {
              last_value = g_EnvCfg.sENV.f.buff[i];
              g_EnvCfg.sENV.f.buff[i]= value;
              if(set_env_flash(g_EnvCfg) == OK)
              {
                sdlog_printf_app("flash set %s ok,value = %f",key_f[i],value);
              }
              else
              {
                g_EnvCfg.sENV.f.buff[i]=last_value;
                sdlog_printf_app("flash set %s Fail,value = %f",key_f[i],value);
              }
            } 
            return;
        }
    }
    sdlog_printf_app("set env Fail,input err ");
    return;
}

/*************************************************
Function: 			logGetEnv
Description:		获取环境参数 如 get_env motor 4
Input:	cmd  :get_env motor
*************************************************/
void logGetEnv(const char* cmd)
{
    uint8_t value = 0;
    uint8_t i = 0;
    char *getValue = NULL;

    for(i=0; i<KEY_NUM; i++)
    {
        if(true == token_cmd((char *)&cmd[7],(char *)&key[i],
                             &value,false))
        {
            getValue = read_env_app((char *)&key[i]);
            if(getValue != NULL)
            {
                sdlog_printf_app("get %s = sd[%s] flash[%d]",key[i],
                                 getValue,g_EnvCfg.sENV.type.buff[i]);
            }
            else
            {
                sdlog_printf_app("get SD %s is NULL,flash=[%d]",
                                 key[i],g_EnvCfg.sENV.type.buff[i]);
            }
            return;
        }
    }
    sdlog_printf_app("get env Fail,input err ");
    return;
}

/*************************************************
Function: 			logGetEnv_float
Description:		获取环境参数 如 get_env_f motor 4
Input:	cmd  :get_env_f joy_a 
*************************************************/
void logGetEnv_float(const char* cmd)
{
    float value = 0;
    uint8_t i = 0;
    char *getValue = NULL;

    for(i=0; i<KEY_F_NUM; i++)
    {
        //get_env_f = 9
        if(true == token_cmd_f((char *)&cmd[9],(char *)&key_f[i],
                             &value,false))
        {
            getValue = read_env_app((char *)&key_f[i]);
            if(getValue != NULL)
            {
                sdlog_printf_app("get %s = sd[%s] flash[%f]",key_f[i],
                                 getValue,g_EnvCfg.sENV.f.buff[i]);
            }
            else
            {
                sdlog_printf_app("get SD %s is NULL,flash=[%f]",
                                 key_f[i],g_EnvCfg.sENV.f.buff[i]);
            }
            return;
        }
    }
    sdlog_printf_app("get env float Fail,input err ");
    return;
}

/*************************************************
Function: 			logDelEnv
Description:		获取环境参数 如 del_env motor 4
Input:	cmd  :del_env motor
*************************************************/
void logDelEnv(const char* cmd)
{
    uint8_t value = 0;
    uint8_t i = 0;

    for(i=0; i<KEY_NUM; i++)
    {
        if(true == token_cmd((char *)&cmd[7],(char *)&key[i],
                             &value,false))
        {
            if(Dele_envinfo_app((char *)&key[i]) == LOG_RW_OK)
            {
                sdlog_printf_app("dell %s ok", key[i]);
            }
            else
            {
                sdlog_printf_app("dell %s fail",key[i]);
            }
            return;
        }
    }
    sdlog_printf_app("det env fail,input err ");
    return;
}
/*************************************************
Function: 			chassisReboot
Description:		系统重启
Input:	cmd
*************************************************/
void chassisReboot(void)
{
    GR_LOG_LINE(" ");
    GR_LOG_INFO("===reboot chassis system===");
    delayUs(500000);//500ms
    __disable_irq();
    HAL_NVIC_SystemReset();
}
/*************************************************
Function: 			Get_Parameter
Description:		获取参数
Input:	cmd
*************************************************/
void Get_Parameter(void)
{
    Joystick_TypeDef joy = {0,0,0,0,0,0};
    float limit_buff[5] = {0};
    float log_size = 0;
    uint32_t err_count;

    getJoystickData(&joy);
    GetSpeedLimitPara(limit_buff);

    log_size  = Get_SD_Size(&SD_INFO);
    err_count = getErrorCnt();

    sdlog_printf_app("log_size=%f,Joy_x=%f,Joy_y=%f,		\
                    lim_en=%f,lim_min=%f,lin_max=%f, 		\
                    ang_min=%f,ang_max=%f,err_cnt=%ld", \
                     log_size, joy.offset_val_x, joy.offset_val_y,
                     limit_buff[0], limit_buff[1], limit_buff[2],
                     limit_buff[3], limit_buff[4], err_count);
    GR_LOG_INFO("log_size=%f,Joy_x=%f,Joy_y=%f,		\
								lim_en=%f,lim_min=%f,lin_max=%f, 		\
								ang_min=%f,ang_max=%f,err_cnt=%ld", \
                log_size, joy.offset_val_x, joy.offset_val_y,
                limit_buff[0], limit_buff[1], limit_buff[2],
                limit_buff[3], limit_buff[4], err_count);
}


/*************************************************
Function:       set_maintain
Description:    设置维护车辆模式开关 [off=0|on=1]
Input:  cmd : [set_maintain 0|1]   eg. set_maintain 0|1
用于应对车体需要人推动它，满足在需要维修的情况下的搬移工作
*************************************************/
void set_maintain(const char* cmd)
{
    uint8_t mod = 0;
    if(true == token_cmd(cmd,"set_maintain",&mod,true))
    {
        if ((mod == ON) || (mod == OFF))
        {
            control_maintenanceMode(mod);
            sdlog_printf_app("set_maintain is = %d ok", mod);
            GR_LOG_INFO("set_maintain is = %d  ok", mod);
            return;
        }
        else
        {
            sdlog_printf_app("set_maintain %d err,input [off|on]=[0|1]", mod);
            GR_LOG_ERROR("set_maintain %d err,input [off|on]=[0|1]", mod);
            return;
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[set_maintain 0|1]");
        GR_LOG_ERROR("please input eg.[set_maintain 0|1]");
    }
}

/*************************************************
Function: 			set_joysMode
Description:		设置摇杆模式开关 [off=0|on=1]
set_joymod  [0/1]
Input:	cmd : [set_joymod 0|1]   eg. set_joymod 0|1
*************************************************/
void set_joysMode(const char* cmd)
{
    uint8_t mod = 0;
    if(true == token_cmd(cmd,"set_joymod",&mod,true))
    {
        if ((mod == ON) || (mod == OFF))
        {
            setJoyMode(mod);
            sdlog_printf_app("set_joymod is = %d ok", mod);
            GR_LOG_INFO("set_joymod is = %d  ok", mod);
            return;
        }
        else
        {
            sdlog_printf_app("set_joymod %d err,input [off|on]=[0|1]", mod);
            GR_LOG_ERROR("set_joymod %d err,input [off|on]=[0|1]", mod);
            return;
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[set_joymod 0|1]");
        GR_LOG_ERROR("please input eg.[set_joymod 0|1]");
    }
}
/*************************************************
Function: 			log_power_ctl
Description:		电源板相关设备电源开关 [off=0|on=1]
power [tx2/break/pad/12v ] [0/1]
Input:	cmd : [power tx2 0|1]
*************************************************/
void log_power_ctl(const char* cmd)
{
    const char power_type[6][6] = {
        "break",
        "pad",
        "tx2",
        "12v",
				"all",
				"ult"
    };
    uint8_t type = 0;
    uint8_t i = 0;
    for(i=0; i<6; i++)
    {
        if(true == token_cmd((char *)&cmd[5],(char *)&power_type[i],
                             &type,true))
        {
            if ((type == ON) || (type == OFF))
            {
                Reboot_Power(i+1,type);
                sdlog_printf_app("power %s %d ok", power_type[i],type);
                GR_LOG_INFO("power %s %d ok", power_type[i],type);
                return;
            }
            else
            {
                sdlog_printf_app("power %s %d err,input [off|on]=[0|1]", power_type[i],type);
                GR_LOG_ERROR("power %s %d err,input [off|on]=[0|1]", power_type[i],type);
                return;
            }
        }
    }

    sdlog_printf_app("please input [power tx2/break/pad/12v  0/1]");
    GR_LOG_ERROR("please input [power tx2/break/pad/12v  0/1]");
    return;
}

/*************************************************
Function: 			ledControl
Description:		灯带控制效果
Input:	cmd :   led 3 100
*************************************************/
void ledControl(const char* cmd)
{
    uint8_t  led_cmd = 0;
    uint32 led_time = 0;
    LedCmd Led;
    if(true == token_value(cmd,"led",&led_cmd,&led_time))
    {
        if(led_cmd > 18)
        {
            sdlog_printf_app("set led is = %d > 18 error", led_cmd);
            GR_LOG_ERROR("set led is = %d > 18 error", led_cmd);
            return;
        }
        else
        {
            //默认最小 250 ms
            if(led_cmd > 9  && led_time < 250 )
            {
                sdlog_printf_app("set led is = [%d %d] < 250 ", led_cmd,led_time);
                //GR_LOG_ERROR("set led is = [%d %d]  < 250 ", led_cmd,led_time);
                led_time = 250; //采用默认
            }
            Led.cmd = led_cmd;
            Led.onoff_ms = led_time;
            Led.hz_ms = led_time;
            sendLedCtl(Led);
            
            //过于频繁 屏蔽
            sdlog_printf_app("set led is = [%d %d]  ok", led_cmd,led_time);
            //GR_LOG_INFO("set led is = [%d %d]  ok", led_cmd,led_time);
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[led 3 500]");
        GR_LOG_ERROR("please input eg.[led 3 500]");
    }

}

/*************************************************
Function: 			set_ultChannel
Description:		设置超声波通道安装使能配置
Input:	cmd :   ult_ch 3 0|1 Enable：1:代表安装  0:代表不安装 
ult_ch通道从 1 开始到 12,
*************************************************/
#if 1
void set_ultChannel(const char* cmd)
{
    char  ult_ch  = 0;
    uint32 ult_en = 0;
    
    UltCh Ult;
    if(true == token_value(cmd,"set_ult_ch",(uint8_t *)&ult_ch,&ult_en))
    {
        if((ult_ch != 0) && ult_ch <= 12 )
        {
           Ult.ch = ult_ch;
           Ult.en = (uint8_t)ult_en;
           sendSetUltCh(Ult);     
        }
        else
        {
            sdlog_printf_app("please set_ult_ch[0<%d<=12] [%d != 0|1] or Already set",ult_ch,ult_en);
            GR_LOG_WARN("please set_ult_ch[0<%d<=12] [%d != 0|1] or Already set",ult_ch,ult_en);
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[set_ult_ch 3 0|1]");
        GR_LOG_ERROR("please input eg.[set_ult_ch 3 0|1]");
    }

}


#else
void set_ultChannel(const char* cmd)
{
    char  ult_ch = 0,last_en = 0;
    uint32 ult_en = 0;
    if(true == token_value(cmd,"set_ult_ch",(uint8_t *)&ult_ch,&ult_en))
    {
        if((ult_ch != 0) && ult_ch <= 12 
          && g_EnvCfg.sENV.ult[ult_ch-1] != (uint8_t)ult_en )
        {
          last_en = g_EnvCfg.sENV.ult[ult_ch-1];
          g_EnvCfg.sENV.ult[ult_ch-1] = (uint8_t)ult_en;
          
          //储存flash
          if(set_env_flash(g_EnvCfg) == OK)
          {
            sdlog_printf_app("flash set_ult_ch[%d] %d  ok",ult_ch,ult_en);
						GR_LOG_INFO("flash set_ult_ch[%d] %d  ok",ult_ch,ult_en);
          }
          else
          {
            g_EnvCfg.sENV.ult[ult_ch-1] = last_en;
            sdlog_printf_app("flash set_ult_ch[%d] %d Fail",ult_ch,ult_en);
						GR_LOG_ERROR("flash set_ult_ch[%d] %d  Fail",ult_ch,ult_en);
          }
               
        }
				else
				{
						sdlog_printf_app("please set_ult_ch[0<%d<=12] [%d != 0|1] or Already set",ult_ch,ult_en);
						GR_LOG_WARN("please set_ult_ch[0<%d<=12] [%d != 0|1] or Already set",ult_ch,ult_en);
				}
    }
    else
    {
        sdlog_printf_app("please input eg.[set_ult_ch 3 0|1]");
        GR_LOG_ERROR("please input eg.[set_ult_ch 3 0|1]");
    }

}
#endif
/*************************************************
Function: 			get_ultChannel
Description:		设置超声波通道安装使能配置
Input:	cmd :   []get_ult_ch 3] 0|1 Enable：1:代表安装  0:代表不安装 
ult_ch通道从 1 开始到 12,
*************************************************/
#if 1
void get_ultChannel(const char* cmd)
{
    uint8_t ult_ch = 0;
    UltCh Ult;
    if(true == token_cmd(cmd,"get_ult_ch",&ult_ch,true))
    {
        if (ult_ch <= 12 && ult_ch != 0)
        {
          Ult.ch = ult_ch;
          Ult.en = 0;
          sendGetUltCh(Ult);
          return;
        }
        else
        {
            sdlog_printf_app("get_ult_ch %d err,input [off|on]=[0|1]", ult_ch);
            GR_LOG_DEBUG("get_ult_ch %d err,input [off|on]=[0|1]", ult_ch);
            return;
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[get_ult_ch 0|1]");
        GR_LOG_ERROR("please input eg.[get_ult_ch 0|1]");
    }
}

#else
void get_ultChannel(const char* cmd)
{
    uint8_t ult_ch = 0;
    if(true == token_cmd(cmd,"get_ult_ch",&ult_ch,true))
    {
        if (ult_ch <= 12 && ult_ch != 0)
        {
          sdlog_printf_app("get_ult_ch[%d] flash read %d",ult_ch,g_EnvCfg.sENV.ult[ult_ch-1]);
          GR_LOG_INFO("get_ult_ch[%d] flash read %d",ult_ch,g_EnvCfg.sENV.ult[ult_ch-1]);
          return;
        }
        else
        {
            sdlog_printf_app("get_ult_ch %d err,input [off|on]=[0|1]", ult_ch);
            GR_LOG_ERROR("get_ult_ch %d err,input [off|on]=[0|1]", ult_ch);
            return;
        }
    }
    else
    {
        sdlog_printf_app("please input eg.[get_ult_ch 0|1]");
        GR_LOG_ERROR("please input eg.[get_ult_ch 0|1]");
    }
}
#endif 
/*************************************************
Function: 			tp_ftp
Description:		ftp任务创建与删除
Input:	cmd :   "ftp creat"   "ftp del"
*************************************************/
void tp_ftp(const char *cmd)
{
	const char creat[] = "ftp creat";
	const char del[] = "ftp del";

  int fsm_cur_state = 0;
  fsm_cur_state = getChassisFsmCurState();

  if ( (fsm_cur_state == CHASSIS_STATE_MANNUAL) 
     ||(fsm_cur_state == CHASSIS_STATE_AUTO) )
  {
    sdlog_printf_app("err chassis is running at mannual or auto ");
    GR_LOG_ERROR("err chassis is running at mannual or auto ");
    return;
  }
    
	if (strncmp(creat, cmd, strlen(creat)) == 0)  //creat ftp 
	{
      //sdlog_printf_app("get ftp cmd .start--Creat ");
      GR_LOG_ERROR("get ftp cmd .start--Creat");
			ftp_CreatTask();
	}
	else if(strncmp(del, cmd, strlen(del)) == 0)  //del ftp
	{
      //sdlog_printf_app("get ftp cmd .start--del ");
      GR_LOG_ERROR("get ftp cmd .start--del");
			ftp_Del_Task();
	}
	else
	{
		sdlog_printf_app("Please input ftp {<creat><del>}");
    GR_LOG_ERROR("ftp input command not found");
	}
}
 
/*************************************************
Function:       ult_reboot
Description:    超声波重启命令
Input:  cmd :   "ult_reboot"   
*************************************************/
void ult_reboot(const char *cmd)
{
    uint8_t value = 0;
    if(true == token_cmd(cmd,"ult_reboot",&value,false))
    {
				//MainBoard_Online_Update_Status.Status = Update_Ok;
        value = 1;  //reboot
        publishFrameData(ULT_ID,PUB_Ult_REBOOT_ID,&value,sizeof(value));
        sdlog_printf_app("ult_reboot ok");
        GR_LOG_INFO("ult_reboot ok");
        set_ult_reboot(ON);
    }
    else
    {
        sdlog_printf_app("please input eg.[ult_reboot ]");
        GR_LOG_ERROR("please input eg.[ult_reboot ]");
    }
}

#ifdef __TP_USER
/*************************************************
Function: 			get_tp_cmd
Description:		解析命令参数
Input:	cmd:命令行字符串
Output: 				none
Return: 				pTP 解析后数据结构体
*************************************************/
bool get_tp_cmd(const char* cmd, pTPcmd pTP)
{
    char tmpBuf[30]= {0};
    char *token = NULL;
    int en = 0;
    int counter = 0;

    if(cmd==NULL)
    {
        sdlog_printf_app("%s", "tp_main input err");
        return false;
    }
    //strlen("tp_main") = 7 + 空格 <tp_main $cmd $tp_id,$enable]>
    //strncpy((char *)tmpBuf,(char *)&cmd[8],30);
    strncpy(tmpBuf, cmd + 8, sizeof(tmpBuf));

    /*获取 $cmd */
    token = strtok(tmpBuf," ");
    if( token != NULL)
    {
        if (strncmp("set_tp", token, strlen("set_tp")) == 0)
        {
            pTP->cmd = eTP_CMD_SET;

            /*获取 $tp_id,*/
            token = strtok(NULL," ");
            if( token != NULL  && (isdDigist(token) == true))
            {
                pTP->id = atoi(token);
            }
            else
            {
                sdlog_printf_app("%s", "set_tp err,please input tp_id is digist");
                return false;
            }

            /*获取 $enable] */
            token = strtok(NULL," ");
            if( token != NULL  && (isdDigist(token) == true))
            {
                en =atoi(token);
                if(en > 1 || en < 0)
                {
                    sdlog_printf_app("set_tp err,please input $enable is [0|1] cur=%d",en);
                    return false;
                }
                else
                {
                    pTP->enable = (en == 1) ? true:false;
                    if (en == 1)
                    {
                        token = strtok(NULL," ");
                        if( token != NULL  && (isdDigist(token) == true))
                        {
                            counter = atoi(token);
                            if (counter > 0 && counter < TP_DEFAULT_CNT)
                            {
                                pTP->cnt = counter;
                            }
                            else
                            {
                                sdlog_printf_app("set_tp err, invalid cnt!");
                                return false;
                            }
                        }
                        else
                        {
                            if(pTP->id == eTP15_comnun_disable)
                            {
                                sdlog_printf_app("please input recovery time(1=10ms 100=1s)");
                                return false;
                            }
                            else
                            {
                                pTP->cnt = TP_DEFAULT_CNT;
                            }
                        }
                    }
                    else
                    {
                        pTP->cnt = TP_DEFAULT_CNT;
                    }
                }
            }
            else
            {
                GR_LOG_ERROR("please input $enable is digist\n");
                sdlog_printf_app("%s", "set_tp err,please input $enable is digist");
                return false;
            }
        }
        else if (strncmp("get_tp", token, strlen("get_tp")) == 0)
        {
            pTP->cmd = eTP_CMD_GET;
            /*获取 $tp_id,*/
            token = strtok(NULL," ");
            if( token != NULL  && (isdDigist(token) == true))
            {
                pTP->id = atoi(token);
            }
            else
            {
                sdlog_printf_app("%s", "get_tp:err,please input tp_id is digist");
                return false;
            }
        }
        else if (strncmp("get_list_tp", token, strlen("get_list_tp")) == 0)
        {
            pTP->cmd = eTP_CMD_LIST;
        }
        else
        {
            sdlog_printf_app("tp_main:err,please input \
          'tp_main $cmd=[set_tp|get_tp|get_list_tp]'");
            return false;
        }
    }
    return true;
}

/*************************************************
Function: 			set_tp
Description:		设置故障注入id的状态
Input:					id ,enable
Output: 				none
Return: 				none
*************************************************/
void set_tp(tp id,bool enable,uint32_t tp_cnt)
{
    if (enable == true)
    {
        if(false == ENABLE_TP(id,tp_cnt))
        {
            //在底层打log
            return ;
        }
    }
    else
    {
        if(false == DISABLE_TP(id,tp_cnt))
        {
            //在底层打log
            return ;
        }
    }
}

/*************************************************
Function: 			get_tp
Description:		获取故障注入id的状态
Input:					id:获取tp点状态
Output: 				none
Return: 				none
*************************************************/
void get_tp(tp id)
{
    eTP_status getStatus;
    getStatus = getOneStatus(id,true);
    if(eTP_INVALID == getStatus)
    {
        sdlog_printf_app("main_board id = %d not found! \n", id);
    }
}

/*************************************************
Function: 			get_list_tp
Description:		获取所有故障注入id的状态
Input:					none
Output: 				none
Return: 				none
*************************************************/
void get_list_tp(void)
{
    getAllStatus_tp();
    sdlog_printf_app("main_board get_list_tp OK");
}

/*************************************************
Function: 			tp_main
Description:		主板处理故障注入函数
Input:					cmd:命令行字符串 $cmd $tp_id,$enable]
Output: 				none {$cmd=[set_tp|get_tp|get_list_tp]}
Return: 				none
*************************************************/
void tp_main(const char* cmd)
{
    sTPcmd TPcmd;
    if (true == get_tp_cmd(cmd,&TPcmd))
    {
        switch(TPcmd.cmd)
        {
        case eTP_CMD_SET:
            set_tp(TPcmd.id,TPcmd.enable,TPcmd.cnt);
            break;
        case eTP_CMD_GET:
            get_tp(TPcmd.id);
            break;
        case eTP_CMD_LIST:
            get_list_tp();
            break;
        default:
            sdlog_printf_app("get_tp_cmd:err,please input '$cmd=[set_tp|get_tp|get_list_tp]'");
            break;
        }
    }
}

#endif
/*************************************************
Function: 			Log_syscall_cmd
Description:		命令行入口函数
Input:					cmd:命令行字符串
Output: 				none
Return: 				none
*************************************************/
void Log_syscall_cmd(const char* cmd)
{
    uint8_t num;
    if(Log_syscall_lookup(cmd, &num))
    {
        switch(num)
        {
        case eLog_help_id:
            Log_help();
            break;
        case eLog_version_id:
            Log_version();
            break;
        case eLog_main_version_id:
            Log_main_version();
            break;
        case eLog_power_version_id:
            Log_power_version();
            break;
        case eLog_joy_version_id:
            Log_joy_version();
            break;
        case eLog_ult_version_id:
            Log_ult_version();
            break;
        case eLog_led_version_id:
            Log_led_version();
            break;
        case eLog_log_sd_id:
            Log_sd_port(cmd);
            break;
        case eLog_log_set_level_id:
            log_set_level(cmd);
            break;
        case eLog_log_get_level_id:
            log_get_level();
            break;
        case eLog_set_env_id:
            logSetEnv(cmd);
            break;
        case eLog_set_env_f_id:
            logSetEnv_float(cmd);
            break;
        case eLog_get_env_id:
            logGetEnv(cmd);
            break;
        case eLog_get_env_f_id:
            logGetEnv_float(cmd);
            break;
        case eLog_del_env_id:
            logDelEnv(cmd);
            break;
        case eLog_reboot_id:
            chassisReboot();
            break;
        case eLog_get_para_id:
            Get_Parameter();
            break;
        case eLog_reset_gyro_id:
            Begin_Gyro_Calibration();
            break;
        case eLog_reset_joys_id:
            sendJoy_reset();
            break;
        case eLog_maintain_id:
            set_maintain(cmd);
            break;
        case eLog_joymod_id:
            set_joysMode(cmd);
            break;
        case eLog_power_ctl_id:
            log_power_ctl(cmd);
            break;
        case elog_led_ctl_id:
            ledControl(cmd);
            break;
        case eLog_set_ult_ch_id:
            set_ultChannel(cmd);
            break;
        case eLog_get_ult_ch_id:
            get_ultChannel(cmd);
            break;
        case eLog_ftp_id:
			      tp_ftp(cmd);
			      break;
        case eLog_ult_reboot_id:
			      ult_reboot(cmd);
			      break;
#ifdef __TP_USER
        case eLog_tp_main_id:
            tp_main(cmd);
            break;
#endif
        default:
            sdlog_printf_app("cmd not found please innput [help]");
            break;
        }
    }
    else
    {
        sdlog_printf_app("cmd not found please input [help]");
    }
}

/*************************************************
Function: 			getSdFlashState
Description:		获取SD卡状态
*************************************************/
static uint8_t sdState = SD_STATE_OK;
uint8_t getSdFlashState(void)
{
    return sdState;
}


void logCmdHanlder(void)
{
#ifdef __LOG

    uint8_t isSpaceFull = SD_SPACE_FULL;
    uint8_t isInset = SD_EXIT;

    static bool bExit = false;	//是否拔了再插入标志
    static bool bReboot = false;

    isInset = check_SD_exit();
    isSpaceFull = Get_Sdtorage_spacestatus();
    
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP61_sdNotConnectErr,isInset,SD_EXIT);
#endif

    //检测SD插入
    if (isInset==SD_INSET)
    {
        setAlarm(eA_SYS,eb13_sdNotConnectErr,ALARM_OK);
        WriteBit(sdState,SD_STATE_EXIT_BIT,OK);
        //检测拔了是否拔了再插入标志
        if(bExit)
            bReboot  = true;
    }
    else
    {
        //SD卡被拔了标志
        bExit = true;
        setAlarm(eA_SYS,eb13_sdNotConnectErr,ALARM_ERR);
        WriteBit(sdState,SD_STATE_EXIT_BIT,ERR);
    }

    if (isSpaceFull == SD_SPACE_FREE)
    {
        setAlarm(eA_SYS,eb08_SdFullInfo,ALARM_OK);
        WriteBit(sdState,SD_STATE_FULL_BIT,OK);
        bExit = false;
    }
    else
    {
        //拔了SD卡，再插上进入空间满告警，需要重启初始化
        if(bReboot)
        {
						GR_LOG_ERROR("SD input $enable is digist\n");
            chassisReboot();
        }
        setAlarm(eA_SYS,eb08_SdFullInfo,ALARM_ERR);
        WriteBit(sdState,SD_STATE_FULL_BIT,ERR);
    }

    if((isInset == SD_INSET) && (isSpaceFull == SD_SPACE_FREE))
    {
        App_log_cmd();
    }

#endif
}

