#include "controller.h"
#include <math.h>
#include "Motor_Control.h"
#include "idea.h"


void IncrementalPidControler(PID_Typedef *pid,float diff_time)
{
    float integral_err_K = 0.0f;
		float diff_buf;
    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];
    pid->err[0] = pid->goal - pid->real;

    pid->integral = pid->err[0]*diff_time;

    integral_err_K =	pid->KI * pid->integral;


		diff_buf = pid->KD*(pid->err[0] - 2.0f*pid->err[1] + pid->err[2])/diff_time;
		limitRange(diff_buf,0.5f);
	
        //增量式PID
		pid->pid_out += pid->KP*(pid->err[0]-pid->err[1]) + integral_err_K + diff_buf;
	


    if(pid->pid_out>1.0f)
        pid->pid_out = 1.0f;
    if(pid->pid_out<-1.0f)
        pid->pid_out = -1.0f;

    if(pid->EN == 0.0f)//禁用PID
        pid->pid_out = 0.0f;
}

void PositionalPidControler(PID_Typedef *pid,float diff_time)
{

    pid->err[1] = pid->err[0];
    pid->err[0] = pid->goal - pid->real;

    pid->integral += pid->KI * pid->err[0]*diff_time;


		limitRange(pid->integral,pid->integral_K_Max);

    pid->pid_out = pid->KP*pid->err[0] + pid->integral + pid->KD*(pid->err[0] - pid->err[1])/diff_time;

		limitRange(pid->pid_out,1);

    pid->lastTimeReal = pid->real;

    if(pid->EN == 0.0f)//禁用PID
        pid->pid_out = 0.0f;
}
void PID_Calc(PID_Typedef *pid)
{
    float k,i,d;
    pid->err[0] = pid->goal - pid->real; //获取当前误差

    /*计算增量*/
    k = pid->KP * pid->err[0]; //位置式p
    pid->integral+=pid->KI *pid->err[0];
    i = pid->integral;
    if(i>(pid->integral_K_Max))
    {
        i = pid->integral_K_Max*0.55f;
        pid->integral = i;
    }
    else if(i<(-pid->integral_K_Max))
    {
        i = -pid->integral_K_Max*0.55f;
        pid->integral = i;
    }
    d=pid->KD*(pid->err[0] -pid->err[1]);
    pid->pid_out = k+i+d;

    limitRange(pid->pid_out,1.0f);

    pid->err[1] = pid->err[0];
}
#define OPEN_K 0.6f
#define OPEN_B 0.08f

void OpenLoopController(PID_Typedef *pid)
{
    float control_data;
    control_data = pid->goal;
    if(control_data>=0.0f)
    {
        pid->pid_out = (control_data * OPEN_K + OPEN_B)/3.0f;

        if(fabs(pid->KI)>0.01f)
        {
            pid->integral = pid->pid_out/pid->KI;
        }
    }
    else
    {
        pid->pid_out = -(-control_data * OPEN_K + OPEN_B)/3.0f;

        if(fabs(pid->KI)>0.01f)
        {
            pid->integral = pid->pid_out/pid->KI;
        }
    }

    if(pid->pid_out>1.0f)
        pid->pid_out = 1.0f;
    if(pid->pid_out<-1.0f)
        pid->pid_out = -1.0f;
}

float	Linear_V_FrontFeed(float arg,MOTORCONTROL_Typedef *motor_arg)
{
    float k ;
    float b ;
    float FrontFeed_Out;
    k = motor_arg->motor_parameter_Ptr->k;				//含义：电压比速度
    b = motor_arg->motor_parameter_Ptr->b;

    if(fabs(arg)<b)
        return 0;

    if(arg>0)
    {
        FrontFeed_Out = (arg - b)*k;
    }
    else
    {
        FrontFeed_Out = -(-arg - b)*k;
    }

//	if(abs(arg)<b)
//	{
//		FrontFeed_Out = 0;
//	}
    return FrontFeed_Out;
}

