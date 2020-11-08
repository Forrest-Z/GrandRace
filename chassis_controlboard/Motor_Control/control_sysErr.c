#include "control_sysErr.h"//系统错误的时候控制行为
//#include "sys_state_task.h"

//extern Motor_TypeDef motor_inst;
//extern Sys_State_TypeDef Sys_State;

int ctl_sysErr_main(ControlData_Typedef *ctlData)
{
    //禁止运动
    static unsigned char sysErrFlag[5] = {0,0,0,0,0};//摇杆 电机 IMU 保留 保留


//	sysErrFlag[0] = Sys_State.st.sensor_state.joystick;
//	sysErrFlag[1] = Sys_State.st.sensor_state.motor&(~0x60);//0x60刹车失效标识是报警不是保护
//	sysErrFlag[2] = Sys_State.st.sensor_state.imu;
//	sysErrFlag[3] = Sys_State.st.sensor_state.imu;
//	sysErrFlag[4] = Sys_State.st.sensor_state.imu;


    if((sysErrFlag[0]+sysErrFlag[1])!=0)//有任何一个异常，禁止运动
    {
        return 1;
    }

    if(sysErrFlag[2]!=0)//imu，禁止转向闭环
    {
        return 2;
    }

    return 0;
}



