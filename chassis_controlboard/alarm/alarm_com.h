/**************************************************************************
 * @file     alarm_com.h
 * @brief    系统所有告警接口。定义处理下位机所有告警
 * @version  V2.0
 * @date     2019-06-08
 * @author   ycy
 ******************************************************************************/
#ifndef __ALARM_COM_H__
#define __ALARM_COM_H__
#include "chassis_config.h"
#include "types.h"
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "protocol.h"

#ifdef __ALARM_USER
#pragma pack(1)

/*==================私有内部接口函数声明====================*/

/*-----------------系统告警定义------------------------------*/
typedef union _SYS_ALARM_
{
    uint64_t  u64_v;
    uint32_t  D[2];
    uint16_t  W[4];
    uint8_t   B[8];
    struct
    {
        __IO uint64_t b00_sensorTaskStackErr: 1; 	//传感器任务栈异常
        __IO uint64_t b01_ctlTaskStackEerr: 1; 		//控制任务栈异常
        __IO uint64_t b02_communTtaskStackErr: 1;	//通讯任务栈异常
        __IO uint64_t b03_stateTaskStackErr: 1;		//状态机任务栈异常
        __IO uint64_t b04_logTaskStackErr: 1;			//日志任务栈异常
        __IO uint64_t b05_cmdTaskStackErr: 1;			//命令接口任务栈异常

        __IO uint64_t b06_memLeakErr: 1; 					//系统内存泄漏异常
        __IO uint64_t b07_memOverFlowErr: 1;			//系统内存溢出异常
        __IO uint64_t b08_SdFullInfo: 1;					//系统SD卡存满
        __IO uint64_t b09_flashBlockBad: 1;				//系统flash坏块标识
        __IO uint64_t b10_SdBlockBad: 1;					//系统SD卡坏块标识
        __IO uint64_t b11_flashCannotUseErr: 1;		//系统flash不可用标识
        __IO uint64_t b12_sdBrokenErr: 1;					//系统SD卡损坏标识
        __IO uint64_t b13_sdNotConnectErr: 1;			//系统SD卡未连接未插入

        __IO uint64_t b14_cpuLoadErr: 1;					//cpu负荷异常
        __IO uint64_t b15_wdgInfo: 1;							//看门狗复位
        __IO uint64_t b16_updateSupportErr: 1;		//系统不支持升级
        __IO uint64_t b17_cpuTempWarn: 1;					//系统cpu温度高
        __IO uint64_t b18_cpuTempErr: 1;					//系统cpu温度超高
        __IO uint64_t b19_heartPacketLossErr: 1;	//心跳通讯丢包
        __IO uint64_t b20_cmdvelTimeOutErr: 1;    //cmdvel接收指令超时
        __IO uint64_t b21: 42;										//预留 bit21-bit63
    } flag;
} unSYS_ALARM;
//ALARM模块定义

//系统告警名称选项
typedef enum _eSYS_ALARM
{
    eb00_sensorTaskStackErr=0,	//传感器任务栈异常
    eb01_ctlTaskStackEerr,			//控制任务栈异常
    eb02_communTtaskStackErr,	//通讯任务栈异常
    eb03_stateTaskStackErr,	 	//状态机任务栈异常
    eb04_logTaskStackErr,	 		//日志任务栈异常
    eb05_cmdTaskStackErr,	 		//命令接口任务栈异常
    eb06_memLeakErr,						//系统内存泄漏异常
    eb07_memOverFlowErr,				//系统内存溢出异常
    eb08_SdFullInfo,						//系统SD卡存满
    eb09_flashBlockBad,	 			//系统flash坏块标识
    eb10_SdBlockBad,						//系统SD卡坏块标识
    eb11_flashCannotUseErr,	 	//系统flash不可用标识
    eb12_sdBrokenErr,	 				//系统SD卡损坏标识
    eb13_sdNotConnectErr,	 		//系统SD卡未连接,未插入

    eb14_cpuLoadErr,						//cpu负荷异常
    eb15_wdgInfo,							//看门狗复位
    eb16_updateSupportErr,			//系统不支持升级
    eb17_cpuTempWarn,					//系统cpu温度高
    eb18_cpuTempErr,						//系统cpu温度超高
    eb19_heartPacketLossErr,  //心跳通讯丢包
    eb20_cmdvelTimeOutErr,    //cmdvel接收指令超时
    eb21_reserve,							//预留 bit20-bit63

} eSYS_ALARM;

/*---系统告警阈值定义---*/
typedef union _SYS_ALARM_CONFIG_
{
    uint16_t  u16_v[8];
    uint8_t   u8_v[16];
    struct
    {
        uint16_t b00_sensorTaskStackErr_v; 	//传感器任务栈异常值
        uint16_t b01_ctlTaskStackEerr_v; 		//控制任务栈异常
        uint16_t b02_communTtaskStackErr_v;	//通讯任务栈异常
        uint16_t b03_stateTaskStackErr_v;		//状态机任务栈异常
        uint16_t b04_logTaskStackErr_v;			//日志任务栈异常
        uint16_t b05_cmdTaskStackErr_v;			//命令接口任务栈异常

        uint8_t b14_cpuLoadErr_v;					//cpu负荷异常
        uint8_t b17_cpuTempWarn_v;					//系统cpu温度高
        uint8_t b18_cpuTempErr_v;					//系统cpu温度超高
        uint8_t b19;												//预留 bit19-bit63
    } cfg;
} unSYS_ALARM_CFG;
/*-================================================================-*/

/*-----------------电机告警定义------------------------------*/
typedef union _MOTOR_ALARM_
{
    uint64_t  u64_v;
    uint32_t  D[2];
    uint16_t  W[4];
    uint8_t   B[8];
    struct
    {
        __IO uint64_t b00_lSpeedDateErr: 1; 		//左电机速度数据异常
        __IO uint64_t b01_lParkBrakeErr: 1;		//左电机驻车刹车失效
        __IO uint64_t b02_rSpeedDateErr: 1;			//右电机速度数据异常
        __IO uint64_t b03_rParkBrakeErr: 1;			//右电机驻车刹车失效
        __IO uint64_t b04_emgyStopFatal: 1;			//急停按下

        __IO uint64_t b05_lHallFatal: 1;				//左电机霍尔错误
        __IO uint64_t b06_lFailMoveFatal: 1; 		//左电机失效(给控制指令但不动)
        __IO uint64_t b07_lOutCtrlFatal: 1;			//左电机失控(电机不受控快速运动)
        __IO uint64_t b08_rHallFatal: 1;				//右电机霍尔错误
        __IO uint64_t b09_rFailMoveFatal: 1; 		//右电机失效(给控制指令但不动)
        __IO uint64_t b10_rOutCtrlFatal: 1;			//右电机失控(电机不受控快速运动)
        __IO uint64_t b11: 52;									//预留 bit11-bit63
    } flag;
} unMOTOR_ALARM;

//电机告警名称选项
typedef enum _eMOTOR_ALARM
{
    eb00_lSpeedDateErr = 0, //左电机速度数据异常
    eb01_lParkBrakeErr,		  //左电机驻车刹车失效
    eb02_rSpeedDateErr,			//右电机速度数据异常
    eb03_rParkBrakeErr, 		//右电机驻车刹车失效
    eb04_emgyStopFatal, 		//急停按下

    eb05_lHallFatal,				//左电机霍尔错误
    eb06_lFailMoveFatal,		//左电机失效(给控制指令但不动)
    eb07_lOutCtrlFatal, 		//左电机失控(电机不受控快速运动)
    eb08_rHallFatal,				//右电机霍尔错误
    eb09_rFailMoveFatal,		//右电机失效(给控制指令但不动)
    eb10_rOutCtrlFatal, 		//右电机失控(电机不受控快速运动)
    eb11_reserve,						//预留 bit11-bit63

} eMOTOR_ALARM;

/*-电机告警定义----*/
//没有需要配置初始化值
//typedef union _MOTOR_ALARM_CONFIG
//{
//    uint8_t   u8_v[1];
//    struct
//    {
//        uint8_t b11;	//预留 bit11-bit63
//    } cfg;
//} unMOTOR_ALARM_CFG;
/*------------------------------------------------------*/

/*-----------------电驱告警定义------------------------------*/
typedef union _DRIVER_ALARM_
{
    uint64_t  u64_v;
    uint32_t  D[2];
    uint16_t  W[4];
    uint8_t   B[8];
    struct
    {
        __IO uint64_t b00_lCommunFatal : 1; 			//左驱动器通讯异常
        __IO uint64_t b01_lHallFatal: 1;					//左驱动器霍尔异常
        __IO uint64_t b02_lOverI_protectFatal: 1;	//左驱动器过流保护
        __IO uint64_t b03_lBlock_protectFatal: 1;	//左电驱堵转保护

        __IO uint64_t b04_rHallFatal: 1;					//右驱动器霍尔异常
        __IO uint64_t b05_rCommunFatal: 1;				//右驱动器通讯异常
        __IO uint64_t b06_rOverI_protectFatal: 1; //右驱动器过流保护
        __IO uint64_t b07_rBlock_protectFatal: 1;	//右电驱堵转保护

        __IO uint64_t b08_rOverHeatErr: 1;				//右驱动器过热
        __IO uint64_t b09_rOverV_Err: 1; 					//右驱动器过压
        __IO uint64_t b10_rUnderV_Err: 1;					//右驱动器欠压
        __IO uint64_t b11_rShortCircuitFatal: 1;	//右驱动器短路
        __IO uint64_t b12_rEmgyStopFatal: 1;			//右驱动器紧急停止
        __IO uint64_t b13_rBrushSensorFatal: 1; 	//右驱动器无刷传感器故障
        __IO uint64_t b14_rMosfetFatal: 1;				//右驱动器MOSFET故障
        __IO uint64_t b15_rStartLoadConfFatal: 1;	//右驱动器启动时加载默认配置

        __IO uint64_t b16_lOverHeatErr: 1;				//左驱动器过热
        __IO uint64_t b17_lOverV_Err: 1;					//左驱动器过压
        __IO uint64_t b18_lUnderV_Err: 1; 				//左驱动器欠压
        __IO uint64_t b19_lShortCircuitFatal: 1;	//左驱动器短路
        __IO uint64_t b20_lEmgyStopFatal: 1;			//左驱动器紧急停止
        __IO uint64_t b21_lBrushSensorFatal: 1; 	//左驱动器无刷传感器故障
        __IO uint64_t b22_lMosfetFatal: 1;				//左驱动器MOSFET故障
        __IO uint64_t b23_lStartLoadConfFatal: 1;	//左驱动器启动时加载默认配置

        __IO uint64_t b24: 39;										//bit24-bit63	预留

    } flag;
} unDRIVER_ALARM;

//电驱告警名称选项
typedef enum _eDRIVER_ALARM
{
    eb00_lCommunFatal = 0, 		//左驱动器通讯异常
    eb01_lHallFatal,					//左驱动器霍尔异常
    eb02_lOverI_protectFatal, //左驱动器过流保护
    eb03_lBlock_protectFatal, //左电驱堵转保护

    eb04_rHallFatal,					//右驱动器霍尔异常
    eb05_rCommunFatal,				//右驱动器通讯异常
    eb06_rOverI_protectFatal, //右驱动器过流保护
    eb07_rBlock_protectFatal, //右电驱堵转保护

    eb08_rOverHeatErr,				//右驱动器过热
    eb09_rOverV_Err,					//右驱动器过压
    eb10_rUnderV_Err, 				//右驱动器欠压
    eb11_rShortCircuitFatal,	//右驱动器短路
    eb12_rEmgyStopFatal,			//右驱动器紧急停止
    eb13_rBrushSensorFatal, 	//右驱动器无刷传感器故障
    eb14_rMosfetFatal,				//右驱动器MOSFET故障
    eb15_rStartLoadConfFatal, //右驱动器启动时加载默认配置

    eb16_lOverHeatErr,				//左驱动器过热
    eb17_lOverV_Err,					//左驱动器过压
    eb18_lUnderV_Err, 				//左驱动器欠压
    eb19_lShortCircuitFatal,	//左驱动器短路
    eb20_lEmgyStopFatal,			//左驱动器紧急停止
    eb21_lBrushSensorFatal, 	//左驱动器无刷传感器故障
    eb22_lMosfetFatal,				//左驱动器MOSFET故障
    eb23_lStartLoadConfFatal, //左驱动器启动时加载默认配置

    eb24_reserve,					//bit24-bit63 预留

} eDRIVER_ALARM;

/*------------------------------------------------------*/


/*-----------------外部传感器告警定义------------------------------*/
typedef union _EXTERNAL_SENSOR_ALARM_
{
    uint64_t  u64_v;
    uint32_t  D[2];
    uint16_t  W[4];
    uint8_t   B[8];
    struct
    {
        __IO uint64_t b00_joysDateErr: 1; 				//摇杆数据异常
        __IO uint64_t b01_joysCommunFatal: 1;			//摇杆通讯异常
        __IO uint64_t b02_joysMcuTempErr: 1;			//摇杆板mcu温度异常
        __IO uint64_t b03_ledCommunWarn: 1;				//灯带模块通讯异常
        __IO uint64_t b04_ledMcuTempWarn: 1;			//灯带mcu温度异常

        __IO uint64_t b05_ultCommunWarn: 1;				//超声波采集板与主控通讯异常
        __IO uint64_t b06_ultChannel_01_warn: 1; 	//超声波通道01异常
        __IO uint64_t b07_ultChannel_02_warn: 1;	//超声波通道02异常
        __IO uint64_t b08_ultChannel_03_warn: 1;	//超声波通道03异常
        __IO uint64_t b09_ultChannel_04_warn: 1; 	//超声波通道04异常
        __IO uint64_t b10_ultChannel_05_warn: 1;	//超声波通道05异常
        __IO uint64_t b11_ultChannel_06_warn: 1; 	//超声波通道06异常
        __IO uint64_t b12_ultChannel_07_warn: 1;	//超声波通道07异常
        __IO uint64_t b13_ultChannel_08_warn: 1;	//超声波通道08异常
        __IO uint64_t b14_ultChannel_09_warn: 1; 	//超声波通道09异常
        __IO uint64_t b15_ultChannel_10_warn: 1;	//超声波通道10异常
        __IO uint64_t b16_ultChannel_11_warn: 1; 	//超声波通道11异常
        __IO uint64_t b17_ultChannel_12_warn: 1;	//超声波通道12异常
        __IO uint64_t b18_joyCheckSumErr_info: 1;	//摇杆数据校验错误
        __IO uint64_t b19_joyCalibrationErr: 1;	  //摇杆标定错误
        __IO uint64_t b20: 43;									  //预留 bit19-bit63
    } flag;
} unEXTE_SENSOR_ALARM;

//外部传感器告警名称选项
typedef enum _eEXTE_SENSOR_ALARM
{
    eb00_joysDateErr = 0, 	 //摇杆数据异常
    eb01_joysCommunFatal = 1,		 //摇杆通讯异常
    eb02_joysMcuTempErr = 2, 		 //摇杆板mcu温度异常
    eb03_ledCommunWarn = 3,			 //灯带模块通讯异常
    eb04_ledMcuTempWarn = 4, 		 //灯带mcu温度异常

    eb05_ultCommunWarn = 5,			  //超声波采集板与主控通讯异常
    eb6_ultChannel_1_warn = 6,  //超声波通道01异常
    eb7_ultChannel_2_warn = 7,  //超声波通道02异常
    eb8_ultChannel_3_warn = 8,  //超声波通道03异常
    eb9_ultChannel_4_warn = 9,  //超声波通道04异常
    eb10_ultChannel_5_warn = 10,  //超声波通道05异常
    eb11_ultChannel_6_warn = 11,  //超声波通道06异常
    eb12_ultChannel_7_warn = 12,  //超声波通道07异常
    eb13_ultChannel_8_warn = 13,  //超声波通道08异常
    eb14_ultChannel_9_warn = 14,  //超声波通道09异常
    eb15_ultChannel_10_warn = 15,  //超声波通道10异常
    eb16_ultChannel_11_warn = 16,  //超声波通道11异常
    eb17_ultChannel_12_warn = 17,  //超声波通道12异常
    eb18_joyCheckSumErr_info = 18, //摇杆数据校验错误
    eb19_joyCalibrationErr = 19,	  //摇杆标定错误
    eb_reserve, 						    //预留 bit18-bit63

} eEXTE_SENSOR_ALARM;

/*------------------------------------------------------*/

/*-----------------内部感器告警定义------------------------------*/
typedef union _INTERNAL_SENSOR_ALARM_
{
    uint32_t  u32_v;
    uint16_t  W[2];
    uint8_t   B[4];
    struct
    {
        __IO uint32_t b00_imuGyro_OverDrift_Err: 1; //imu陀螺仪漂移过大
        __IO uint32_t b01_imuGyro_OverNoise_Err: 1;	//imu陀螺仪噪声过大
        __IO uint32_t b02_imuDataFatal: 1;					//imu数据异常
        __IO uint32_t b03_imuCommunHdwFatal: 1;			//imu硬件/通讯错误
        __IO uint32_t b04_imuTempErr: 1;						//imu温度异常
        __IO uint32_t b05_m4TempErr: 1;						  //m4片内温度异常
        __IO uint32_t b06: 26;									    //预留 bit06-bit32
    } flag;
} unINTE_SENSOR_ALARM;

//内部感器告警名称选项
typedef enum _eINTERNAL_SENSOR_ALARM
{
    eb00_imuGyro_OverDrift_Err=0, //imu陀螺仪漂移过大
    eb01_imuGyro_OverNoise_Err,   //imu陀螺仪噪声过大
    eb02_imuDataFatal,					  //imu数据异常
    eb03_imuCommunHdwFatal, 		  //imu硬件/通讯错误
    eb04_imuTempErr,						  //imu温度异常
    eb05_m4TempErr,						    //imu温度异常
    eb06_reserve,							    //预留 bit05-bit32

} eINTERNAL_SENSOR_ALARM;
    
/*--内部感器告警定义配置--*/
typedef union _INTERNAL_ALARM_CONFIG
{
    uint8_t   u8_v[2];//
    struct
    {
        uint8_t b04_imuTempErr_v;  //imu温度异常
        uint8_t reserve;          //bit25-bit31 预留
    } cfg;
} unINTERNAL_ALARM_CFG;

/*------------------------------------------------------*/
/*-----------------电源系统告警定义------------------------------*/
typedef union _POWER_ALARM_
{
    uint32_t  u32_v;
    uint16_t  W[2];
    uint8_t   B[4];
    struct
    {
        __IO uint32_t b00_bmsPowerLow_warn: 1; 		//bms低电量标识
        __IO uint32_t b01_bmsTempLow_warn: 1;			//bms低温标识
        __IO uint32_t b02_bmsTempHigt_warn: 1;		//bms高温标识
        __IO uint32_t b03_powerHightI_warn: 1;		//高电流标识
        __IO uint32_t b04_powerLowV_warn: 1;			//低电压标识

        __IO uint32_t b05_main2powerCommun_err: 1;		//电源板与主控通讯断开标识
        __IO uint32_t b06_power2bmsCommun_err: 1;			//电源板与bms通讯断开标识

        __IO uint32_t b07_ultraLow_PowerCapa_err: 1; 	//超低电量标识
        __IO uint32_t b08_ultraLow_Temp_err: 1;				//超低温标识
        __IO uint32_t b09_ultraHight_Temp_err: 1;			//超高温标识
        __IO uint32_t b10_ultraHight_I_err: 1;				//超高电流标识
        __IO uint32_t b11_ultraHight_V_err: 1;				//超低电压标识

        __IO uint32_t b12_12v_inside_V_warn: 1;		//12V内部电压异常
        __IO uint32_t b13_tx2_V_warn: 1;					//TX2电压异常
        __IO uint32_t b14_5v_V_warn: 1;						//5V电压异常
        __IO uint32_t b15_12v_out_V_warn: 1;			//12V外部电压异常

        __IO uint32_t b16_12v_inside_V_fatal: 1;	//12V内部电压故障
        __IO uint32_t b17_tx2_V_fatal: 1;					//TX2电压故障
        __IO uint32_t b18_5v_V_fatal: 1;					//5V电压故障
        __IO uint32_t b19_12v_out_V_fatal: 1;			//12V外部电压故障
        __IO uint32_t b20_pad_overI_fatal: 1;			//PAD供电过流
        __IO uint32_t b21_out_overI_fatal: 1;			//外设供电过流(超声波/路由器/RTK)

        __IO uint32_t b22_joys_overI_fatal: 1;	//摇杆供电过流
        __IO uint32_t b23_powerTempWarn: 1;			//电源板温度高
        __IO uint32_t b24_powerTempErr: 1;	    //电源板温度超高
        __IO uint32_t b25: 6;									  					//bit25-bit31	预留
    } flag;
} unPOWER_ALARM;

//电源系统告警名称选项
typedef enum _ePOWER_ALARM
{
    eb00_bmsPowerLow_warn=0,		//bms低电量标识
    eb01_bmsTempLow_warn, 			//bms低温标识
    eb02_bmsTempHigt_warn,			//bms高温标识
    eb03_powerHightI_warn,			//高电流标识
    eb04_powerLowV_warn,				//低电压标识

    eb05_main2powerCommun_err,		//电源板与主控通讯断开标识
    eb06_power2bmsCommun_err, 		//电源板与bms通讯断开标识

    eb07_ultraLow_PowerCapa_err,	//超低电量标识
    eb08_ultraLow_Temp_err, 			//超低温标识
    eb09_ultraHight_Temp_err, 		//超高温标识
    eb10_ultraHight_I_err,				//超高电流标识
    eb11_ultraHight_V_err,				//超低电压标识

    eb12_12v_inside_V_warn, 	//12V内部电压异常
    eb13_tx2_V_warn,					//TX2电压异常
    eb14_5v_V_warn, 					//5V电压异常
    eb15_12v_out_V_warn,			//12V外部电压异常

    eb16_12v_inside_V_fatal,	//12V内部电压故障
    eb17_tx2_V_fatal, 				//TX2电压故障
    eb18_5v_V_fatal,					//5V电压故障
    eb19_12v_out_V_fatal, 		//12V外部电压故障
    eb20_pad_overI_fatal, 		//PAD供电过流
    eb21_out_overI_fatal, 		//外设供电过流(超声波/路由器/RTK)

    eb22_joys_overI_fatal,	//摇杆供电过流
    eb23_powerTempWarn,			//电源板温度高
    eb24_powerTempErr, 			//电源板温度超高
    eb25_reserve,						//bit25-bit31 预留

} ePOWER_ALARM;

/*--电源系统告警定义--*/
typedef union _POWER_ALARM_CONFIG
{
    uint8_t   u8_v[16];//4+4*2=12
    struct
    {
        int b01_bmsTempLow_warn_v;			//bms低温标识
        uint8_t b02_bmsTempHigt_warn_v;		//bms高温标识

        int b08_ultraLow_Temp_err_v;				//超低温标识
        uint8_t b09_ultraHight_Temp_err_v;	//超高温标识

        uint8_t b23_powerTempWarn_v;			//电源板温度高
        uint8_t b24_powerTempErr_v;	      //电源板温度超高
        //uint8_t b25: 6;									//bit25-bit31	预留
    } cfg;
} unPOWER_ALARM_CFG;

/*------------------------------------------------------*/
/*-----------------chassis告警定义------------------------------*/
typedef union _CHASSIS_ALARM_
{
    uint64_t  u64_v[5];
    uint32_t  u32_v[10];
    uint16_t  u16_v[20];
    uint8_t   u8_v[40];
    struct
    {
        unSYS_ALARM    sys; 			//系统告警定义 64 bit 8
        unMOTOR_ALARM  motor;			//电机告警定义 64 bit 8
        unDRIVER_ALARM driver;		//电驱告警定义 64 bit 8
        unEXTE_SENSOR_ALARM exte; //外部传感器告警定义 64 bit 8
        unINTE_SENSOR_ALARM inte; //内部感器告警定义  32 bit 4
        unPOWER_ALARM power;			//电源系统告警定义  32 bit 4
    } alarm;
} unCHASSIS_ALARM;




/*=========================================================*/
/*==================外部公共接口函数声明====================*/
typedef union _PUB_ALARM_
{
    uint8_t   buff[52]; //[40+12 = 52]
    struct
    {
        Time_StampTypeDef time;	//发布告警定义时间戳  		[12]
        unCHASSIS_ALARM 	unA;	//发布告警定义内容     		  [40] unAlarm
    } sA;//sAlarm
} unPUB_ALARM;

/*-----------------chassis告警阈值定义------------------------------*/
typedef union _CHASSIS_ALARM_CONFIG
{
    uint8_t   buff[32];//16+16
    struct
    {
        unSYS_ALARM_CFG   sys; 		//系统告警定义 			16
        unPOWER_ALARM_CFG power;	//电源系统告警定义  16
        unINTERNAL_ALARM_CFG inte;//内部传感器配置
    } sA;
} unCHASSIS_ALARM_cfg;
/*==========================================================*/

extern unPUB_ALARM g_sAlarmPub;//告警定义
extern unCHASSIS_ALARM_cfg g_cfg;//告警阈值定义
#else
extern int  g_sAlarmPub;//告警定义
#endif // __ALARM_USER

//对外接口
#define ALARM_OK  	0 		//正常值
#define ALARM_ERR 	1			//异常值

//ALARM类别项
typedef enum _eALARM_MOD
{
    eA_SYS = 0, //系统告警定义
    eA_MOTOR,		//电机告警定义
    eA_DRIVER,	//电驱告警定义
    eA_EXTE,		//外部传感器告警定义
    eA_INTE,		//内部感器告警定义
    eA_POWER,		//电源系统告警定义
} eALARM_MOD;

#define MOD_NUM  6

//设置模块告警
void setAlarm(eALARM_MOD _mod,uint8_t _bit,uint8_t _flag);
//获取模块告警
uint8_t getAlarm(eALARM_MOD _mod,uint8_t _bit);
//清除模块告警，即设置模块无告警
void clearAlarm(eALARM_MOD _mod);
//初始化告警
void initAlarm(void);
//告警是否有数据在变化
bool alarmMonitor(void);
#endif

