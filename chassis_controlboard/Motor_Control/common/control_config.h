#ifndef _CONTROL_CONFIG_H
#define _CONTROL_CONFIG_H
#include "Motor_PcConfig.h"
/*2018/12/17 wqw 这是一个关于控制模块的参数配置文件
原则：
	1.所有的控制模块内部的配置都在这里面
	2.参数按参数分层罗列
快速包含：
	//#include "control_config.h"
*************************************************************************/
/*
参数分层

	电机类型（相当于轮子类型）

	电机驱动类型

	底盘类型

	控制层参数
*************************************************************************
电机接口数据结构定义
*/

/********************参数定义***********************/

#define pi 3.1425926f

#define LEFT_MOTOR    0
#define RIGHT_MOTOR   1

#define CTRL_T 0.01f

/*电机类型参数
*******************************************/
#define MOTOR_ORIGINAL 1
#define MOTOR_ZHAOYANG 2
#define MOTOR_JIQIN90N 3
#define MOTOR_LIANYI60N 4
#define MOTOR_LIANYI_NEW 5
#define MOTOR_MAX 5
#define MOTOR_TYPE_DEFAULT MOTOR_LIANYI_NEW

/******************************************/

#if MOTOR_TYPE == MOTOR_ORIGINAL
#define DIAMETER_WHEEL 0.244f											    //轮子直径 单位m		
#define SCALE_WHEEL		(91*3)													//轮子转动一周,霍尔传感器获得的上升沿个数
#define Number_of_motor_pole 8.0f
#define Reduction_ratio			 23.0f
#endif

#if MOTOR_TYPE == MOTOR_ZHAOYANG
#define DIAMETER_WHEEL 0.304f
#define SCALE_WHEEL		(45.0f*3.0f)
#define Number_of_motor_pole 20.0f
#define Reduction_ratio			 4.4f
#endif

#if MOTOR_TYPE == MOTOR_JIQIN90N
#define DIAMETER_WHEEL 0.304f
#define SCALE_WHEEL		(45.0f*3.0f)
#define Number_of_motor_pole 20.0f
#define Reduction_ratio			 9f
#endif

#if MOTOR_TYPE == MOTOR_LIANYI60N
#define DIAMETER_WHEEL 0.304f
#define SCALE_WHEEL		(75.0f*3.0f)
#define Number_of_motor_pole 10.0f
#define Reduction_ratio			 15.0f
#endif

#if MOTOR_TYPE == MOTOR_LIANYI_NEW
#define DIAMETER_WHEEL 0.304f
#define SCALE_WHEEL		(75.0f*3.0f)
#define Number_of_motor_pole 10.0f
#define Reduction_ratio			 15.0f
#endif

/*电机驱动类型
****************************************************/
#define BLD_IO 		1
#define AQMDBL 		2
#define KEYA			3
#define MOTER_DEIVER_MAX			3

#define MOTER_DEIVER_TYPE KEYA

#define MOTOR_DRIVER_VOLTAGE_CTL 					0		//电压控制型
#define MOTOR_DRIVER_CURRENT_CTL 					1		//力矩控制型
#define MOTOR_DRIVER_INTERNAL_SELF_CTL 		2		//内部自闭环

/*电机驱动类型end
****************************************************/


/*
底盘类型
***************************************************/
#define WHEEL_CHAIR1 	0
#define WHEEL_CHAIR2 	1
#define ROAM 					2

#define CHASSIS_MAX 	2

#define CHASSIS_TYPE ROAM

#if CHASSIS_TYPE == WHEEL_CHAIR1
#define WHEEL_SEPARATION                 0.55f           // meter
#endif
#if CHASSIS_TYPE == WHEEL_CHAIR2
#define WHEEL_SEPARATION                 0.562f           // meter
#endif
#if CHASSIS_TYPE == ROAM	//漫游	文旅车
#define WHEEL_SEPARATION                 1.065f           // meter
#endif

/*
控制层参数
*********************************/
#define MIN_LINEAR_SPEED  0.05f
#define MIN_ANGULAR_SPEED 0.05f

#define MAX_LINEAR_FRONT_SPEED 	2.77f
#define MAX_LINEAR_BACK_SPEED 	-0.3f
#define MAX_ANGULAR_SPEED 1.0f

#define MAX_FRONT_ACC   50.2f
#define MAX_FRONT_DEC   0.8f

#define MAX_BACK_ACC    1.2f
#define MAX_BACK_DEC    0.8f

#define MAX_ANGULAR_ACC 1.2f
#define MAX_ANGULAR_DEC 0.8f

#define MAX_MOTOR_SPEED 3.0f	//执行器的动态范围属于它固有物理属性，不应该在程序中作限制

extern float LEFT_PID_KP;
extern float LEFT_PID_KI;
extern float LEFT_PID_KD;

extern float RIGHT_PID_KP;
extern float RIGHT_PID_KI;
extern float RIGHT_PID_KD;

extern float ANGULAR_PID_KP;
extern float ANGULAR_PID_KI;
extern float ANGULAR_PID_KD;


/*
电机接口数据结构
*********************************/

#define MOTOR_STATE_ERR_ABNORM_L		(0x01<<0)
#define MOTOR_STATE_ERR_BRK_FAIL_L	(0x01<<1)
#define MOTOR_STATE_ERR_ABNORM_R		(0x01<<2)
#define MOTOR_STATE_ERR_BRK_FAIL_R	(0x01<<3)
#define MOTOR_STATE_ERR_SCRAM				(0x01<<4)
#define MOTOR_STATE_ERR_HALL_L			(0x01<<5)
#define MOTOR_STATE_ERR_FAIL_L			(0x01<<6)
#define MOTOR_STATE_ERR_OUT_CTR_L		(0x01<<7)
#define MOTOR_STATE_ERR_HALL_R			(0x01<<8)
#define MOTOR_STATE_ERR_FAIL_R			(0x01<<9)
#define MOTOR_STATE_ERR_OUT_CTR_R		(0x01<<10)

#define MOTOR_DRIVER_ERR_COM_L						(0x01<<0)
#define MOTOR_DRIVER_ERR_HALL_L						(0x01<<1)
#define MOTOR_DRIVER_ERR_OVERCURRENT_L		(0x01<<2)
#define MOTOR_DRIVER_ERR_LOCKED_ROTOR_L		(0x01<<3)
#define MOTOR_DRIVER_ERR_HALL_R						(0x01<<4)
#define MOTOR_DRIVER_ERR_COM_R						(0x01<<5)
#define MOTOR_DRIVER_ERR_OVERCURRENT_R		(0x01<<6)
#define MOTOR_DRIVER_ERR_LOCKED_ROTOR_R		(0x01<<7)
#define MOTOR_DRIVER_ERR_SUPERHEAT_L			(0x01<<8)
#define MOTOR_DRIVER_ERR_SUPERHEAT_R			(0x01<<9)
#define MOTOR_DRIVER_ERR_MOS_L						(0x01<<10)
#define MOTOR_DRIVER_ERR_MOS_R						(0x01<<11)
#define MOTOR_DRIVER_ERR_UNDERVOLTAGE			(0x01<<12) //欠压  
#define MOTOR_DRIVER_ERR_OVERVOLTAGE			(0x01<<13) //过压 
#define MOTOR_DRIVER_ERR_LOAD_CFG			    (0x01<<14) //启动时加载的默认配置错误
#define MOTOR_DRIVER_VOLTAGE_NOT_STABLE		(0x01<<15) //驱动器电压不稳定


#define MOTOR_DRIVER_TIME_OUT 		500

#pragma pack(1)
typedef struct
{
    unsigned int state;
    //bit0	左电机速度数据异常
    //bit1	左电机驻车刹车失效
    //bit2	右电机速度数据异常
    //bit3	右电机驻车刹车失效
    //bit4	急停按下
    //bit5	左电机霍尔错误
    //bit6	左电机失效(给控制指令但不动)
    //bit7	左电机失效(电机不受控快速运动)
    //bit8	右电机霍尔错误
    //bit9	右电机失效(给控制指令但不动)
    //bit10	右电机失效(电机不受控快速运动)
    //bit11-bit15	预留
    unsigned int motor_driver;
    //bit0	左驱动器通讯异常
    //bit1	左驱动器霍尔异常
    //bit2	左驱动器过流保护
    //bit3	左电驱堵转保护
    //bit4	右驱动器通讯异常
    //bit5	右驱动器霍尔异常
    //bit6	右驱动器过流保护
    //bit7	右电驱堵转保护
		//bit15  驱动器电压不稳定
    //bit8-bit15	预留

    unsigned char EN;				//驱动器使能
    unsigned char BRK;				//驱动器刹车
    unsigned char lock;				//驻车刹车
    unsigned char lock_Disable;		//驻车刹车 失能
    unsigned char scramFlag;		//急停标志 为1代表处于急停状态(驱动器是否供电)
    unsigned char HALLFlag[2];  	//霍尔标志

    unsigned char prefilter_EN;				//前置滤波器使能

    float set_speed[2];								//轮子设定速度
    float real_speed[2];							//编码器采集到的速度
    float distance[2];								//编码器计数
    float elec[2];										//电机电流
		float input_voltage;												//驱动器输入电压
    float driver_import[2];						//驱动器执行量入口
		float Ts;													//最近一次的采样周期

    unsigned int timeOut[2];						//驱动器响应超时 默认500ms value = 0.5/Ts

    //参数指针
    MOTOR_PARAMETER_    *motor_parameter_Ptr;
    CHASSIS_PARAMETER_  *chassis_parameter_Ptr;
    DRIVER_PARAMETER_   *driver_parameter_Ptr;
} MOTORCONTROL_Typedef;

#endif
