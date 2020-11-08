/***************************************************
 * @file     trmp_sensor.c
 * @brief    读取CPU内部温度。
 * @version  V2.9
 * @date     2019-08-09
 * @author   wqw
 ****************************************************/
#include "temp_sensor.h"
#include "imu.h"
#include <string.h>
#include "fault_injector_wrapper.h"
#include "alarm_com.h"
#include "SD_Log_App.h"

/*************************************
2019/8/9
对M4的温度分三个板子做了测量
	电压		温度
	0.779			37.5
	0.812			51
	0.7964		46.5
	0.782			37.0
	0.83			56.5
	0.802			45.0
	0.794			42.2
	0.776			34.8
	0.810			50
	0.828			57
	标准差为	1.272
	区3倍标准差，M4温度测量误差为+-3.8摄氏度
**************************************/

//内部参考电压1.21V典型电压值
#define VREFIN 1.21								

Temp_Typedef temp_sensor;
float adc_data;
float v_ref;

/*************************************************
Function: 			init_temp_sensor
Description:		初始化定义 外部接口
Return: 				none
*************************************************/
void init_temp_sensor(void)
{
  //秒速计数初始值10 十秒之后才稳定值 1hz更新一次
	temp_sensor.count = 10;
}

/*************************************************
Function: 			get_Vref
Description:		更新参考电压值
Return: 				none
*************************************************/
void get_Vref(void)
{
	
	if(temp_sensor.count==5)
		v_ref = adc_data;
	else
		v_ref += 0.5f*(adc_data - v_ref);
	
	if(temp_sensor.count == 2)
	{
		ADC_ChannelConfTypeDef sConfig;
		sConfig.Channel = ADC_CHANNEL_16;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
		{
			_Error_Handler(__FILE__, __LINE__);
		}
	}
	temp_sensor.count--;
}

/*************************************************
Function: 			read_temp_sensor
Description:		初始化定义
Return: 				none
*************************************************/
void read_temp_sensor()
{
	ImuPara_TypeDef imu_sensor;
	getImuData(&imu_sensor);	
	temp_sensor.IMU = imu_sensor.temp;  //获得IMU的温度
	
	HAL_ADC_Start(&hadc1);	
	adc_data = HAL_ADC_GetValue(&hadc1); 
	if(temp_sensor.count != 0)
	{
    //获取更新参考电压
		get_Vref();
	}
	else
	{
		adc_data = adc_data/v_ref*1.21f;							//换算成了真实电压值
		temp_sensor.M4 = adc_data*408.6f - 281.5f;		//获得stm32f4温度				
	}	

}

/*************************************************
Function: 			get_temp_sensor
Description:		获取芯片imu和芯片M4内部温度值
output: temp_ptr:温度值指针
Return: none
*************************************************/
void get_temp_sensor(Temp_Typedef* temp_ptr)
{
	 memcpy(temp_ptr,&temp_sensor,sizeof(Temp_Typedef));
}

/*************************************************
Function: 			getM4Temp
Description:		获取芯片m4内部温度值
output: none
Return: temp 无符号值温度
*************************************************/
uint8_t getM4Temp(void)
{
    static bool b_log = false;
    uint8 temp = 0;
    temp = (uint8_t)(temp_sensor.M4);
    
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP59_cpuTempWarn,temp, (g_cfg.sA.sys.cfg.b17_cpuTempWarn_v +1 ));
    TP_MODIFY_VALUE(eTP60_cpuTempErr,temp, (g_cfg.sA.sys.cfg.b18_cpuTempErr_v +1 ));
#endif  
    if (temp > g_cfg.sA.sys.cfg.b18_cpuTempErr_v && b_log == false)
    {
			 b_log = true;
			 GR_LOG_ERROR("m4 cpu temp error [%d]",temp);
			 setAlarm(eA_SYS,eb18_cpuTempErr,ALARM_ERR);      
    }
    else if (temp > g_cfg.sA.sys.cfg.b17_cpuTempWarn_v && b_log == false)
    {
			 b_log = true;
			 GR_LOG_ERROR("m4 cpu temp hight [%d]",temp);
			 setAlarm(eA_SYS,eb17_cpuTempWarn,ALARM_ERR);
    }
    else  if(temp <= g_cfg.sA.sys.cfg.b17_cpuTempWarn_v && b_log == true)
    {
        b_log = false;
        setAlarm(eA_SYS,eb17_cpuTempWarn,ALARM_OK);
        setAlarm(eA_SYS,eb18_cpuTempErr,ALARM_OK);
        GR_LOG_WARN("m4 temp recover [%d]",temp);
    }

    return temp;
}

/*************************************************
Function: 			getImuTemp
Description:		获取芯片imu内部温度值
output: none
Return: temp 无符号值温度
*************************************************/
uint8_t getImuTemp(void)
{
    uint8 temp = 0;
    temp = (uint8_t)(temp_sensor.IMU);
    return temp;
}


