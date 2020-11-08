#include "Motor_Control_Monitor.h"
#include "motor_driver.h"
#include "Publish_powerboard.h"
unsigned int M_C_monitorCounter = 0;		//用于测量执行频率
ControlData_Typedef* M_C_controlData_Ptr;

void M_C_MonitorInit(ControlData_Typedef *controlData)
{
    HAL_TIM_Base_Start_IT(&htim14);

    M_C_controlData_Ptr = controlData;
}

void M_C_MonitorInterface(ControlData_Typedef *controlData)
{
    M_C_monitorCounter ++;

}

void M_C_MonitorCallback(TIM_HandleTypeDef *htim)//1s中断一次
{   //中断尽可能的执行更短的代码

    if(M_C_monitorCounter<1)//小于1Hz
    {
        //关闭控制
        M_C_controlData_Ptr->goalLinear_v_pre = 0;
        M_C_controlData_Ptr->goalAngular_v_pre = 0;

        M_C_controlData_Ptr->motor_inst_Ptr->driver_import[0] = 0;
        M_C_controlData_Ptr->motor_inst_Ptr->driver_import[1] = 0;
        M_C_controlData_Ptr->motor_inst_Ptr->BRK = 1;
        M_C_controlData_Ptr->motor_inst_Ptr->EN = 1;
        M_C_controlData_Ptr->motor_inst_Ptr->lock = 1;

//        writeMotorControlData(M_C_controlData_Ptr->motor_inst_Ptr);
    }

    M_C_monitorCounter = 0;
}

/*
用于监护驻车刹车的电源。因为驻车刹车的电源可能会有损坏的危险，这种情况下，驻车刹车会不能闭合。通过关闭驻车刹车电源来补偿这个问题
*/
void M_C_MonitorLockPower(ControlData_Typedef *controlData)
{
	static unsigned int lastLockState;
	unsigned int latestLockState;
	
	static float oldStamp;
	float newStamp;
	
	latestLockState = controlData->motor_inst_Ptr->state&(MOTOR_STATE_ERR_BRK_FAIL_L|MOTOR_STATE_ERR_BRK_FAIL_R);
	
	newStamp = ( (float)getSystemTick() ) / 1000.0f;
	
	if((latestLockState != lastLockState)||(newStamp - oldStamp)>1.0f)
	{
		oldStamp = newStamp;
		if(latestLockState)//发生了故障
		{
			Reboot_Power(break_power,OFF);
		}
		else
		{
			Reboot_Power(break_power,ON);
		}
	}

	lastLockState = latestLockState;
}

