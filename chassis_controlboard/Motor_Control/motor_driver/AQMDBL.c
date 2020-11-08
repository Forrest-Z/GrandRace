#include "AQMDBL.h"

static unsigned char motor_state = 0;

float distance_offset[2] = {0,0};

unsigned char AQMDBL_initMotorControlData(MOTORCONTROL_Typedef *ctrl_msg)
{

//	HAL_Delay(1000);//等待电机驱动初始化完成，若在电机初始化之前，发送数据给电机驱动会导致电机驱动无法启动
//
//
//
//	Init_NewMotor_Communication();
//	//HAL_Delay(500);
//	Set_Motor_PIDParameter();
//	Set_Motor_ControlType();
//	Set_LeftRight_Motor_RatedCurrent();
//	Set_LeftRight_Motor_MaxLoadCurrent();
//	Set_LeftRightMotor_Parameter();
//	Read_MotorStudy_Status();
//	Read_MotorWorkType();
//	//HAL_Delay(500);
//	Read_PID_Parameter();

    return 0;
}

unsigned char AQMDBL_writeMotorControlData(MOTORCONTROL_Typedef *ctrl_msg)
{
//	static float buf[2];
//	Set_Motor_Speed(buf[LEFT_MOTOR],Left_Motor);
//	Set_Motor_Speed(buf[RIGHT_MOTOR],Right_Motor);
    static unsigned char lock_count = 0;//用于避免继电器频繁切换

    if(ctrl_msg->lock)
    {   //刹车需要等0.5s
        lock_count++;
        if(lock_count>50)
        {
            lock_count = 50;
            AQMDBL_EN_Lock(ctrl_msg->lock);
        }
    } else { //松开刹车是立刻的
        lock_count = 0;
        AQMDBL_EN_Lock(ctrl_msg->lock);
    }
//	Set_Motor_Speed(ctrl_msg->set_speed[LEFT_MOTOR],Left_Motor);
//	Set_Motor_Speed(ctrl_msg->set_speed[RIGHT_MOTOR],Right_Motor);
    return 0;
}

extern MOTOR_STATUS motor_status;
extern MOTOR_TOALE_TRIP motor_toale_trip;
extern MOTOR_REALSPEED motor_speed;
unsigned char AQMDBL_readMotorControlData(MOTORCONTROL_Typedef *ctrl_msg)
{

//	Read_MotorStatus();
//	Read_MotorRealSpeed();
//	Read_Total_Trip();

//	ctrl_msg->real_speed[LEFT_MOTOR]  = motor_speed.LeftMotor_RealSpeed;
//	ctrl_msg->real_speed[RIGHT_MOTOR] = motor_speed.RightMotor_RealSpeed;
//	if( (motor_toale_trip.LeftMotor_Toale_Trip
//		+ motor_toale_trip.RightMotor_Toale_Trip) == 0 )										//使电机发生复位后不清除里程
//	{
//		distance_offset[LEFT_MOTOR] = ctrl_msg->distance[LEFT_MOTOR];
//		distance_offset[RIGHT_MOTOR] = ctrl_msg->distance[RIGHT_MOTOR];
//	}else
//	{
//		ctrl_msg->distance[LEFT_MOTOR]    = motor_toale_trip.LeftMotor_Toale_Trip + distance_offset[LEFT_MOTOR];
//		ctrl_msg->distance[RIGHT_MOTOR]   = motor_toale_trip.RightMotor_Toale_Trip + distance_offset[RIGHT_MOTOR];
//	}
//	AQMDBL_scramFlag(&ctrl_msg->scramFlag);
    return 0;
}

unsigned char AQMDBL_getMotorState(void)
{
    unsigned char scramStatus;
    AQMDBL_scramFlag(&scramStatus);
    if(scramStatus==1)
    {
        motor_state |= 0x02;
    } else
    {
        motor_state &= ~0x02;
    }
    return motor_state;
}

