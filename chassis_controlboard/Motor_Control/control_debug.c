#include "control_debug.h"
#include "Control_selfmodify.h"
#include "Motor_Control.h"
#include "OD.h"
#include <string.h>
#include "SD_Log_App.h"


ControlData_Typedef control_debug_data;

void updateControlDebugdata(ControlData_Typedef *control_data_ptr)	//因为形参拷贝一次，memcpy一次，会出现两次拷贝，所以改为指针
{
    memcpy(&control_debug_data, control_data_ptr, sizeof(ControlData_Typedef));
}
void publishControlDebugInfo(void)
{
	Control_Pub_Buffer.goal_v_speed        = control_debug_data.goalLinear_v;
	Control_Pub_Buffer.goal_a_speed        = control_debug_data.goalAngular_v;
	Control_Pub_Buffer.goal_v_speed_pre    = control_debug_data.goalLinear_v_pre;
	Control_Pub_Buffer.goal_a_speed_pre    = control_debug_data.goalAngular_v_pre;
	Control_Pub_Buffer.present_v_speed     = (control_debug_data.present_v[0] + control_debug_data.present_v[1])/2;
	Control_Pub_Buffer.present_a_speed     = control_debug_data.presentAngular_v;

		
//	Control_Pub_Buffer.goal_left_speed     = LeftWheel_PID.KP;
//	Control_Pub_Buffer.present_left_speed  = LeftWheel_PID.KI;
//	
//	Control_Pub_Buffer.goal_right_speed    = Angular_PID.KP;
//	Control_Pub_Buffer.present_right_speed = Angular_PID.EN;
	

	Control_Pub_Buffer.goal_left_speed     = control_debug_data.wheelSpeed[0];   
	Control_Pub_Buffer.present_left_speed  = control_debug_data.present_v[0];

	Control_Pub_Buffer.goal_right_speed    = control_debug_data.wheelSpeed[1];   
	Control_Pub_Buffer.present_right_speed = control_debug_data.present_v[1];


	Control_Pub_Buffer.present_left_pwm    = control_debug_data.motor_inst_Ptr->driver_import[0];
	Control_Pub_Buffer.present_right_pwm   = control_debug_data.motor_inst_Ptr->driver_import[1];
	Control_Pub_Buffer.present_left_elec = control_debug_data.motor_inst_Ptr->elec[0];
	Control_Pub_Buffer.present_right_elec  = control_debug_data.motor_inst_Ptr->elec[1];

	publishFrameData(TX2_ID,CONTROL_DEBUG_ID, (unsigned char *)&Control_Pub_Buffer, sizeof(Control_VelTypeDef));
}

void setPIDPara(void)
{
    if(Control_COF_Sub_Buffer.enable_control_debug_flag == 1)
    {
        Control_COF_Sub_Buffer.enable_control_debug_flag = 0;	//增加立刻赋0

        Angular_PID.EN = (unsigned char)Control_COF_Sub_Buffer.enable_angular_speed_control_flag;

        Angular_PID.KP = Control_COF_Sub_Buffer.angular_speed_P;
        Angular_PID.KI = Control_COF_Sub_Buffer.angular_speed_I;
        Angular_PID.KD = Control_COF_Sub_Buffer.angular_speed_D;

        LeftWheel_PID.KP = Control_COF_Sub_Buffer.motor_driver_P;
        LeftWheel_PID.KI = Control_COF_Sub_Buffer.motor_driver_I;
        LeftWheel_PID.KD = Control_COF_Sub_Buffer.motor_driver_D;

        RightWheel_PID.KP = Control_COF_Sub_Buffer.motor_driver_P;
        RightWheel_PID.KI = Control_COF_Sub_Buffer.motor_driver_I;
        RightWheel_PID.KD = Control_COF_Sub_Buffer.motor_driver_D;
        Init_FuzzyPID();
    }
}

/*************************************************
Function: 			printf_motor_I
Description:		记录电机电流日志
Output:					none
*************************************************/
void printf_motor_I(void)
{
    if(getSystemTick() > 30000 
      && ((control_debug_data.motor_inst_Ptr->elec[0] > 0 )
        || (control_debug_data.motor_inst_Ptr->elec[1] > 0)))
      GR_LOG_ERROR("L_elec_I=%f R_elec_I=%f",control_debug_data.motor_inst_Ptr->elec[0], control_debug_data.motor_inst_Ptr->elec[1]);
}

