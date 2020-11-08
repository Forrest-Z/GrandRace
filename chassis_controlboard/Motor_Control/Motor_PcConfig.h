#ifndef _Motor_PcConfig_H
#define _Motor_PcConfig_H
#include "imu_driver.h"
#include <stdlib.h>


//用于支持初始化的底盘配置


#pragma pack(1)

//电机配置参数
typedef struct {
    unsigned char motor_type;												//电机类型
    float diameter_wheel;											    //轮子直径 单位m
    int scale_wheel;															//轮子转动一周,霍尔传感器获得的上升沿个数
    float Number_of_motor_pole_;										//电机极对数
    float Reduction_ratio_;													//电机减速比
    float k;																				//电机KV值 的 倒数
    float b;																				//电机曲线 过零点值
    float Encoder_K;																//编码器修正系数 (真实比当前编码器的速度)
} MOTOR_PARAMETER_;

//底盘配置参数
typedef struct {
    unsigned char chassis_type;											//底盘类型
    float wheel_separation;													//后轮轴距
    IMU_PARAMETER imu_parameter_Ptr;								//IMU参数 {安装参数}
} CHASSIS_PARAMETER_;

//驱动器配置参数
typedef struct {
    unsigned char motor_driver_type;					//驱动器类型
    //见宏定义 MOTER_DEIVER_TYPE

    unsigned char motor_driver_control_type;	//驱动器控制方式类型
    //0: 电压控制型
    //1：力矩控制型
    //2：内部闭环型
} DRIVER_PARAMETER_;


MOTOR_PARAMETER_ *Read_MotorType(void);
DRIVER_PARAMETER_ *Read_MotorDriverType(void);
CHASSIS_PARAMETER_ *Read_ChassisType(void);

float reset_odom_encoder(float encoder);
void upDataEncoder_K(float *pEncoder_K);
char* read_motor_para(char* key_word);
void defaultMotorPara(void);

#endif
