/**************************************************************************
 * @file     chassis_com.c
 * @brief    底盘公共函数接口，自实现。
 * @version  V2.0
 * @date     2019-05-31
 * @author   ycy
 ******************************************************************************/
#include "chassis_com.h"
#include <string.h>
#include <stdio.h>
#include "SD_Log_App.h"
#include "flash_if.h"
#include <math.h>
#include "control_config.h"

unENV_CFG g_EnvCfg = {0};

/*************************************************
Function: 			isdDigist
Description:		判断字符串是否为数字
Input:					*pStr:输入字符串
Output: 				none
Return: 				true false
*************************************************/
bool isdDigist( const char *pStr)
{
    const char *p = pStr;
    if (p == NULL ) return false;
    while( *p != 0 )
    {
        if (((*p < '0') || (*p > '9') ) && (*p != '.') )
        {
            return false;
        }
        p++;
    }
    return true;
}
/*************************************************
Function: 			printfChassisVersion
Description:		获取版本号，并打印日志
*************************************************/
void printfChassisVersion(void)
{
//打印版本信息
    GR_LOG_INFO("[%s %s %s];", __DATE__, __TIME__, MAIN_VERSION);
}


/*************************************************
Function: 			token_cmd
Description:		解析命令参数
Input:	_cmd_i:接收到的cmd _cmd_o:与目标cmd比较
_bRead ：是否获取参数值 true 获取 false不获取
Output: 				_out_v 解析后的数据
Return: 				解析是否正确
*************************************************/
bool token_cmd(const char* _cmd_i,const char* _cmd_o,
               uint8 *_out_v,bool _bRead)
{
    char tmpBuf[30]= {0};
    char *token = NULL;

    if(_cmd_i==NULL)
    {
        sdlog_printf_app("%s input err",_cmd_o);
        GR_LOG_ERROR("%s input err",_cmd_o);
        return false;
    }
    //log_level 4
    strncpy(tmpBuf,_cmd_i,sizeof(tmpBuf));

    /*获取 $cmd */
    token = strtok(tmpBuf," ");
    if( token != NULL)
    {
        if ( strlen(token) == strlen(_cmd_o) &&
                strncmp(_cmd_o, token, strlen(_cmd_o)) == 0)
        {
            if(_bRead)
            {
                /*获取 *_out_v,*/
                token = strtok(NULL," ");
                if( token != NULL  && (isdDigist(token) == true))
                {
                    *_out_v = atoi(token);
                    return true;
                }
                else
                {
                    sdlog_printf_app("%s input is not digist", _cmd_o);
                    GR_LOG_ERROR("%s input is not digist", _cmd_o);
                    return false;
                }
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }
    return false;
}

/*************************************************
Function:       token_cmd_f
Description:    解析命令参数
Input:  _cmd_i:接收到的cmd _cmd_o:与目标cmd比较
_bRead ：是否获取参数值 true 获取 false不获取
Output:   _out_f 解析后的数据
Return:   解析是否正确
*************************************************/
bool token_cmd_f(const char* _cmd_i,const char* _cmd_o,
                 float *_out_f,bool _bRead)
{
    char tmpBuf[30]= {0};
    char *token = NULL;

    if(_cmd_i==NULL)
    {
        sdlog_printf_app("%s input err",_cmd_o);
        GR_LOG_ERROR("%s input err",_cmd_o);
        return false;
    }

    strncpy(tmpBuf,_cmd_i,sizeof(tmpBuf));

    /*获取 $cmd */
    token = strtok(tmpBuf," ");
    if( token != NULL)
    {
        if ( strlen(token) == strlen(_cmd_o) &&
                strncmp(_cmd_o, token, strlen(_cmd_o)) == 0)
        {

            if(_bRead)
            {
                /*获取 *_out_v,*/
                token = strtok(NULL," ");
                if( token != NULL  && (isdDigist(token) == true))
                {
                    *_out_f = atof(token);
                    return true;
                }
                else
                {
                    sdlog_printf_app("%s input is not digist", _cmd_o);
                    GR_LOG_ERROR("%s input is not digist", _cmd_o);
                    return false;
                }
            }
            else
            {
                return true;
            }   
        }
        else
        {
            return false;
        }
    }
    return false;
}

/*************************************************
Function:       token_value
Description:    解析命令参数
Input:  _cmd_i:接收到的cmd _cmd_o:与目标cmd比较
Output:         _out_v1 解析后的数据
                _out_v2 解析后的数据
Return:         解析是否正确
*************************************************/
bool token_value(const char* _cmd_i,const char* _cmd_o,
                 uint8 *_out_v1,uint32 *_out_v2)
{
    char tmpBuf[30]= {0};
    char *token = NULL;

    if(_cmd_i==NULL)
    {
        sdlog_printf_app("%s input err",_cmd_o);
        GR_LOG_ERROR("%s input err",_cmd_o);
        return false;
    }
    //log_level 4
    strncpy(tmpBuf,_cmd_i,sizeof(tmpBuf));

    /*获取 $cmd */
    token = strtok(tmpBuf," ");
    if( token != NULL)
    {
        if ( strlen(token) == strlen(_cmd_o) &&
                strncmp(_cmd_o, token, strlen(_cmd_o)) == 0)
        {
            /*获取 *_out_v1*/
            token = strtok(NULL," ");
            if( token != NULL  && (isdDigist(token) == true))
            {
                *_out_v1 = atoi(token);

                /*获取 *_out_v2,*/
                token = strtok(NULL," ");
                if( token != NULL  && (isdDigist(token) == true))
                {
                    *_out_v2 = atoi(token);
                }
                else
                {
                    sdlog_printf_app("%s input second parameter is not digist", _cmd_o);
                    GR_LOG_ERROR("%s input second parameter is not digist", _cmd_o);
                    return false;
                }
            }
            else
            {
                sdlog_printf_app("%s input is not digist", _cmd_o);
                GR_LOG_ERROR("%s input is not digist", _cmd_o);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

/*************************************************
Function: 			initComFlashEnv();
Description:		环境变量配置flash初始化
*************************************************/
void initComFlashEnv(void)
{
    HAL_Delay(100);
    get_env_flash(&g_EnvCfg);
    g_EnvCfg.sENV.type.sTYPE.res_type = 0x89;

    //检查flash读取是否正确 检查读取越界
    if( (g_EnvCfg.sENV.type.sTYPE.motor_type == 0)
      ||(g_EnvCfg.sENV.type.sTYPE.motor_type > MOTOR_MAX)
      ||(g_EnvCfg.sENV.type.sTYPE.driver_type == 0)
      ||(g_EnvCfg.sENV.type.sTYPE.driver_type > MOTER_DEIVER_MAX)
      ||(g_EnvCfg.sENV.type.sTYPE.chassis_type == 0)
      ||(g_EnvCfg.sENV.type.sTYPE.chassis_type > CHASSIS_MAX))
    {   
  		  HAL_Delay(100);
        get_env_flash(&g_EnvCfg);

        if((g_EnvCfg.sENV.type.sTYPE.motor_type > MOTOR_MAX)
          ||(g_EnvCfg.sENV.type.sTYPE.motor_type == 0))
        {
            g_EnvCfg.sENV.type.sTYPE.motor_type = MOTOR_TYPE_DEFAULT;
        }
          
        if(g_EnvCfg.sENV.type.sTYPE.driver_type > MOTER_DEIVER_MAX
          || (g_EnvCfg.sENV.type.sTYPE.driver_type == 0)) 
        {
          g_EnvCfg.sENV.type.sTYPE.driver_type = MOTER_DEIVER_TYPE;
        }
          
        if (g_EnvCfg.sENV.type.sTYPE.chassis_type > CHASSIS_MAX
          || (g_EnvCfg.sENV.type.sTYPE.chassis_type == 0))
        {
          g_EnvCfg.sENV.type.sTYPE.chassis_type = CHASSIS_TYPE;
        }
     }
		 HAL_Delay(100);
}

/*************************************************
Function: 			set_env_flash();
Description:		设置保存环境变量配置
return : ret 0:OK 1:err
*************************************************/
uint32_t set_env_flash(unENV_CFG cfg)
{
    uint32_t ret = 0;
    unENV_CFG env_flash;
    
    FLASH_If_Init();
    memcpy(env_flash.u8_v,cfg.u8_v,sizeof(unENV_CFG));
   
    env_flash.sENV.type.sTYPE.res_type = 0x89;
    ret=FLASH_Env_Write(ENV_FLASH_SAVE_ADDR,env_flash.u32_v,(sizeof(unENV_CFG)/4));
    HAL_Delay(50);

    return ret;
}

/*************************************************
Function: 			get_env_flash();
Description:		读取环境变量配置
*************************************************/
void get_env_flash(unENV_CFG *cfg)
{
    unENV_CFG env_flash;
    stm32_flash_Read(ENV_FLASH_SAVE_ADDR,env_flash.u32_v,(uint32_t)(sizeof(unENV_CFG)/4));
    memcpy(&cfg->u8_v[0],&env_flash.u8_v[0],sizeof(unENV_CFG));
}

/*************************************************
Function: 			get_float_SDkey
Description:		获取环境浮点参数 
Input:	dataName : 关键值 如reset_odom joy_l joy_a
Output: *value 返回浮点值
return true | false
*************************************************/
bool get_float_SDkey(const char *dataName,float *value)
{
    char *str;
    if(dataName==NULL)
    {
        return false;
    }
    str = read_env_app((char*)dataName);
		if(str == NULL)
		{
			return false;
		}				
		*value = atof(str);
    return true;
}

/*************************************************
Function: 			set_float_SDkey
Description:		设置环境浮点参数 
Input:	dataName : 关键值 如reset_odom joy_l joy_a
        arg:设置浮点值
return true | false
*************************************************/
bool set_float_SDkey(const char *dataName,float arg)
{
    char strValue[5] = {0};
    if(dataName==NULL)
    {
        return false; 
    }

    sprintf(strValue, "%f", arg);
    if(write_env_app((char *)dataName,strValue) == EF_RW_OK)
    {
      return true; 
    }
    return false;  
}

