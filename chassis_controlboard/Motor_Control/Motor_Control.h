#ifndef __MOTOR_CONTROL_H__
#define __MOTOR_CONTROL_H__
#include "tim.h"
#include "joystick.h"
#include "motor_driver.h"
#include "control_config.h"
#include "controller.h"
//#include "OD.h"

#pragma pack(4)
typedef struct
{
		float Ts;													//最近一次的采样周期		
    float goalLinear_v;
    float goalAngular_v;

    float present_v[2];
    float presentAngular_v;

    float goalLinear_v_pre;
    float goalAngular_v_pre;

    float wheelSpeed[2];

    float Max_front_acc;
    float Max_front_dec;

    float Max_back_acc;
    float Max_back_dec;

    float Max_angular_acc;
    float Max_angular_dec;

    int joy_BRK;						//摇杆的刹车标志
    int Fuzzy_Control_Enable_flag;

    MOTORCONTROL_Typedef *motor_inst_Ptr;
} ControlData_Typedef;


extern PID_Typedef LeftWheel_PID;
extern PID_Typedef RightWheel_PID;
extern PID_Typedef Angular_PID;
extern ControlData_Typedef controlDataInst;

/*对外函数*********************************************************/

void Init_Motor(void);
int control_main(int);

//切换维护模式
//void control_maintenanceMode(void);
void control_maintenanceMode(uint8_t cmd);

/*内部函数************************************************/

//	初始化
void initControl(void);
//	驻车逻辑
int Brake_Motor(void);
//  控制输入源
void	Goal_And_Sensor_Updata(void);
//	控制逻辑
int	Control_Logic(void);
//	数据预处理
void	Control_Preprocess(void);
//	计算控制值
void	Calculate_Wheel_Speed(void);
//	电机控制
void Wheel_Control(void);

void updateControldata(ControlData_Typedef *copy_Ptr);
//void setAccParameter(Control_COFTypeDef parameter);
uint8_t getParkingState(void);

#endif


