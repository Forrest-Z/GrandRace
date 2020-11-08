/**************************************************************************
 * @file     fault_injector.h
 * @brief    故障注入
 * @version  V2.0
 * @date     2019-04-25
 * @author   ycy
 ******************************************************************************/
#ifndef __FAULT_H__
#define __FAULT_H__
#include "tim.h"
#include "types.h"
#include "protocol.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include "chassis_config.h"
#include "chassis_list.h"

#ifdef __TP_USER


#define TP_LOCK 		1
#define TP_UNLOCK 	0


#pragma pack(1)

//=================================================//
typedef unsigned char tp;

//模块TP点管理 tp
typedef enum {
    eTP0_reboot 	=0,	//软件复位
    eTP1_CPU_100	=1,		//CPU满载 100%
    eTP2_CPU_90	=2,			//CPU满载 90% 
    eTP3_CPU_85	=3,			//CPU满载 850%
    eTP4_SD_FULL	=4,		//存储占满 used_size full
    eTP5_bms_ultra_low_temp =5,			//超低温
    eTP6_bms_ultra_high_temp=6,			//超高温
    eTP7_stack_sensor 	=7,			//传感器任务剩余堆栈量
    eTP8_stack_control =8,			//控制任务剩余堆栈量
    eTP9_stack_comm =9 ,				//通讯任务剩余堆栈量
		eTP10_stack_fsm =10,				//状态机任务剩余堆栈量
		eTP11_stack_log =11,				//日志任务剩余堆栈量

    eTP12_stack_over =12,		//堆栈溢出
    eTP13_mem_leak   =13,		//内存泄漏 暂时无法实现
		eTP14_wdg_rest,					//看门狗复位
	
    eTP15_comnun_disable,		//通讯断开
    eTP16_tx2_heart_packet_loss,	//心跳通讯丢包
    eTP17_bms_low_cap,				//低电量
    eTP18_bms_low_temp,			//bms低温标识
    eTP19_bms_high_temp,		//bms高温标识
    eTP20_high_current,	//高电流标识
    eTP21_low_volotage,	//低电压标识
    eTP22_bms_ultralow_cap,			//超低电量标识 
    eTP23_ultrahigh_current,	//超高电流标识 
    eTP24_ultralow_volotage,	//超低电压标识  
    eTP25_out12v_error,				//12V外部电压异常
    eTP26_tx2_volotage_error,	//TX2电压异常
    eTP27_5v_error,			//5V电压异常
    eTP28_in12v_error,	//12V内部电压异常
    eTP29_pad_current_error,	//PAD供电过流
    eTP30_rtk_current_error,	//外设供电过流(超声波/路由器/RTK)
    eTP31_joy_current_error,	//摇杆供电过流
		
    eTP32_imu_err_hardware,	//imu硬件/通讯错误
    eTP33_imu_err_drift,		//imu陀螺仪漂移过大
    eTP34_imu_err_noise,		//imu陀螺仪噪声过大

    eTP35_driver_right_err_overheat,			//右驱动器过热
    eTP36_driver_right_err_overvoltage,		//右驱动器过压
    eTP37_driver_right_err_undervoltage,	//右驱动器欠压
    eTP38_driver_right_err_short_circuit,	//右驱动器短路
    eTP39_driver_right_err_emergency_stop,//右驱动器紧急停止
    eTP40_driver_right_err_sensor,				//右驱动器无刷传感器故障
    eTP41_driver_right_err_sensor,				//右驱动器MOSFET故障
    eTP42_driver_right_err_load_config,		//右驱动器启动时加载默认配置
    eTP43_driver_left_err_overheat,				//左驱动器过热
    eTP44_driver_left_err_overvoltage,		//左驱动器过压
    eTP45_driver_left_err_undervoltage,		//左驱动器欠压
    eTP46_driver_left_err_short_circuit,	//左驱动器短路
    eTP47_driver_left_err_emergency_stop,//左驱动器紧急停止
    eTP48_driver_left_err_sensor,					//左驱动器无刷传感器故障
    eTP49_driver_left_err_mos,						//左驱动器MOSFET故障
    eTP50_driver_left_err_load_config,		//左驱动器启动时加载默认配置
    eTP51_driver_left_CommunFatal,				//左驱动器通讯异常
    eTP52_driver_right_CommunFatal,				//右驱动器通讯异常
    
    eTP53_joy_data_err,				//摇杆数据异常
    eTP54_joy_checksum_err,	  //摇杆校验错误
    eTP55_imu_temp_hight_err,	//imu温度高异常
    eTP56_main2powerCommun_err,	//电源板与主控通讯断开标识
    eTP57_power2bmsCommun_err,	//电源板与bms通讯断开标识
    eTP58_ledCommunWarn,	    //灯带模块通讯异常
    eTP59_cpuTempWarn,	    //系统cpu温度高
    eTP60_cpuTempErr,				//系统cpu温度超高
    eTP61_sdNotConnectErr,	//系统SD卡未连接接触不良
    eTP62_cmdvelTimeout,	  //接收控制命令超时
    eTP63_joyCalibrationErr,//摇杆标定错误告警
    eTP64_powerTempWarn,		//电源板温度高
    eTP65_powerTempErr,	    //电源板温度超高
} eTM_ID;


//跟踪点的使能
typedef enum _eTRACE_POINT_STATUS
{
    eTP_DISABLED = 0,
    eTP_ENABLED,
    eTP_INVALID,
} eTP_status;

//自定义消息结构体
typedef struct _LIST_MSG_TP_
{
    tp				  id; 		// 故障点标识符
    eTP_status  status; // 故障点的状态
    //char  desc[64];		/* description of tp 去掉 描述*/
    uint32_t cnt;				//执行计数
    struct list_head list;
} slist_msg_tp,*pListMsgTp;

//
//
typedef enum _eTP_CMD
{
    eTP_CMD_SET = 0, //设置tp点
    eTP_CMD_GET, 		//设置tp点
    eTP_CMD_LIST,
} eTP_CMD;
		
//两天计数 按每次10ms 2*24*3600*1000/10
#define TP_DEFAULT_CNT 17280000 
typedef struct _FAULT_TRACE_POINT_CMD
{
    eTP_CMD cmd;	//set_tp get_tp get_list_tp 模块命令
    tp id;				//tp 故障id
    bool enable; 	// 使能
    uint32_t cnt;
} sTPcmd,*pTPcmd;

/* ========================================== */
void initFault(void);
bool createFault(tp tp_id);
bool enable(tp tp_id,uint32_t tp_cnt);
bool disable(tp tp_id,uint32_t tp_cnt);
eTP_status getStatus(tp tp_id,bool isPrint);
void getAllStatus(void);
bool changeStatus(tp tp_id, eTP_status newStatus,uint32_t tp_cnt);
bool isCanBeExecTp(tp tp_id);

/* ========================================== */
#endif


#endif

