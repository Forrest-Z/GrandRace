
#include "motor_driver.h"
#include <string.h>
#include "motor_driver_common.h"
#include "SD_Log_App.h"

#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

static MOTORCONTROL_Typedef motor_drive_inst;

uint8_t canDataBuf[8];

unsigned char initMotorDriver(MOTORCONTROL_Typedef *ctrl_msg)
{
		memcpy(&motor_drive_inst,ctrl_msg,sizeof(MOTORCONTROL_Typedef));

    switch(motor_drive_inst.driver_parameter_Ptr->motor_driver_type)
    {
    case BLD_IO:
        BLD_IO_Init(&motor_drive_inst);
        break;
    case KEYA:
        init_KEYA(&motor_drive_inst);
        break;
    default:
        BLD_IO_Init(&motor_drive_inst);
        break;
    }
    return 0;
}

unsigned char writeMotorControlData(MOTORCONTROL_Typedef *ctrl_msg)
{
    motor_drive_inst.EN = ctrl_msg->EN;
    motor_drive_inst.BRK = ctrl_msg->BRK;
    motor_drive_inst.lock = ctrl_msg->lock;
		motor_drive_inst.lock_Disable = ctrl_msg->lock_Disable;
    motor_drive_inst.set_speed[0] = ctrl_msg->set_speed[0];
    motor_drive_inst.set_speed[1] = ctrl_msg->set_speed[1];
    motor_drive_inst.scramFlag = ctrl_msg->scramFlag;

    /*不同电机的控制兼容*************************************************************/
    switch(motor_drive_inst.motor_parameter_Ptr->motor_type)
    {
    case MOTOR_LIANYI60N:
        motor_drive_inst.driver_import[0] = ctrl_msg->driver_import[0];
        motor_drive_inst.driver_import[1] = ctrl_msg->driver_import[1];
        break;
		case MOTOR_LIANYI_NEW:
				motor_drive_inst.driver_import[0] = -ctrl_msg->driver_import[0];
				motor_drive_inst.driver_import[1] = -ctrl_msg->driver_import[1];
				break;
    default:
        motor_drive_inst.driver_import[0] = ctrl_msg->driver_import[0];
        motor_drive_inst.driver_import[1] = ctrl_msg->driver_import[1];
        break;
    }

    /*end*不同电机的控制兼容************************************************************/


    if((motor_drive_inst.state & (MOTOR_STATE_ERR_FAIL_L|MOTOR_STATE_ERR_FAIL_R)) != 0 
			||motor_drive_inst.motor_driver!=0 )
    {
        motor_drive_inst.driver_import[0] = 0;
        motor_drive_inst.driver_import[1] = 0;
        motor_drive_inst.lock = 1;
    }
		
		if(motor_drive_inst.lock_Disable==1) //如果驻车刹车被禁止
		{
			motor_drive_inst.driver_import[0] = 0;
      motor_drive_inst.driver_import[1] = 0;
			motor_drive_inst.lock = 0;
		}
	
    switch(motor_drive_inst.driver_parameter_Ptr->motor_driver_type)
    {
    case BLD_IO:
        BLD_IO_Write(&motor_drive_inst);
        break;
    case KEYA:
        writeSpeed_KEYA(motor_drive_inst.driver_import[0],KEYA_ADDR_LEFT);
        writeSpeed_KEYA(motor_drive_inst.driver_import[1],KEYA_ADDR_RIGHT);
        break;
    default:
        BLD_IO_Write(&motor_drive_inst);
        break;
    }

    Motor_Driver_Common_Write(&motor_drive_inst);

    return 0;
}


unsigned char readMotorDevice(void)
{
		static uint8_t b_lComLog=false,b_rComLog=false;
    switch(motor_drive_inst.driver_parameter_Ptr->motor_driver_type)
    {
    case BLD_IO:
        BLD_IO_Read(&motor_drive_inst);
        break;
    case KEYA:
        motor_drive_inst.real_speed[0] = readSpeed_KEYA(KEYA_ADDR_LEFT);
        motor_drive_inst.real_speed[1] = readSpeed_KEYA(KEYA_ADDR_RIGHT);

        motor_drive_inst.distance[0] = readDistance_KEYA(KEYA_ADDR_LEFT);
        motor_drive_inst.distance[1] = readDistance_KEYA(KEYA_ADDR_RIGHT);

        motor_drive_inst.elec[0] = readElec_KEYA(KEYA_ADDR_LEFT);
        motor_drive_inst.elec[1] = readElec_KEYA(KEYA_ADDR_RIGHT);
		
				motor_drive_inst.input_voltage = readVolt_KEYA();

        motor_drive_inst.motor_driver = readErr_KEYA();
        break;
    default:
        BLD_IO_Read(&motor_drive_inst);
        break;
    }
/*兼容联谊新电机
***********************************************************************/		
//		switch(motor_drive_inst.motor_parameter_Ptr->motor_type)
//    {
//		case MOTOR_LIANYI_NEW:
//				motor_drive_inst.real_speed[0] *= -1;
//				motor_drive_inst.real_speed[1] *= -1;
//				motor_drive_inst.distance[0]  *= -1;
//        motor_drive_inst.distance[1]  *= -1;
//				break;
//    default:        
//        break;
//    }
/*兼容联谊新电机
***********************************************************************/				

		readVoltageState(&motor_drive_inst);
		
    motor_drive_inst.state = readMotorState(&motor_drive_inst);

    if(motor_drive_inst.scramFlag)//如果按下了急停，清除电机错误标志
    {
        clearMotorState();
    }

    motor_drive_inst.timeOut[0]++;
    motor_drive_inst.timeOut[1]++;
#ifdef __TP_USER
		TP_MODIFY_VALUE(eTP51_driver_left_CommunFatal,motor_drive_inst.timeOut[0],(MOTOR_DRIVER_TIME_OUT+1));
		TP_MODIFY_VALUE(eTP52_driver_right_CommunFatal,motor_drive_inst.timeOut[1],(MOTOR_DRIVER_TIME_OUT+1));
#endif
    if(motor_drive_inst.timeOut[0]>MOTOR_DRIVER_TIME_OUT)
    {
        motor_drive_inst.motor_driver |= MOTOR_DRIVER_ERR_COM_L;
				setAlarm(eA_DRIVER, eb00_lCommunFatal, ALARM_ERR);
				if(!b_lComLog)
				{
						GR_LOG_ERROR("motor driver left communation fatal.");
						b_lComLog= true;
				}
    }
    else
    {
        motor_drive_inst.motor_driver &= ~MOTOR_DRIVER_ERR_COM_L;	
				setAlarm(eA_DRIVER, eb00_lCommunFatal, ALARM_OK);
        if(b_lComLog)
				{
						GR_LOG_WARN("motor driver left commu fatal.recoer");
						b_lComLog= false;
				}
    }
    if(motor_drive_inst.timeOut[1]>MOTOR_DRIVER_TIME_OUT)
    {
        motor_drive_inst.motor_driver |= MOTOR_DRIVER_ERR_COM_R;
				setAlarm(eA_DRIVER, eb05_rCommunFatal, ALARM_ERR);
				if(!b_rComLog)
				{
						GR_LOG_ERROR("motor driver right communation fatal.");
						b_rComLog= true;
				}
    }
    else
    {
        motor_drive_inst.motor_driver &= ~MOTOR_DRIVER_ERR_COM_R;
				setAlarm(eA_DRIVER, eb05_rCommunFatal, ALARM_OK);
				if(b_rComLog)
				{
						GR_LOG_WARN("motor driver right commu fatal.recoer");
						b_rComLog= false;
				}
    }
		
//获取执行周期		 
	getMotorTs();
		
  return 0;
}

float getMotorTs(void)
{
	static float lastStamp_motor;	
	float newStamp_motor;
	newStamp_motor = ( (float)getSystemTick() ) / 1000.0f;
	motor_drive_inst.Ts = newStamp_motor - lastStamp_motor;
	lastStamp_motor = newStamp_motor;	
	return motor_drive_inst.Ts;//返回最近的两次调用的时间间隔 (s)
}

unsigned char updateMotorData(MOTORCONTROL_Typedef *ctrl_msg)
{
    memcpy(ctrl_msg, &motor_drive_inst, sizeof(MOTORCONTROL_Typedef));
    return 0;
}

uint16_t getMotorState(void)
{
    return motor_drive_inst.state;
}
uint16_t getMotorDriverState(void)
{
    return motor_drive_inst.motor_driver;
}

void motorDriverCallback(CAN_HandleTypeDef *hcan)
{
    HAL_StatusTypeDef	HAL_RetVal;
    CAN_RxHeaderTypeDef RxMeg;

    
    if(getSystemTick() < 9000)
      return;
      
    HAL_RetVal=HAL_CAN_GetRxMessage(&hcan2, 0, &RxMeg,  canDataBuf);

    if(HAL_OK!=HAL_RetVal)
    {
        return;
    }

    switch(motor_drive_inst.driver_parameter_Ptr->motor_driver_type)
    {
    case KEYA:
        canCallback_KEYA(canDataBuf);
        motor_drive_inst.timeOut[0] = 0;
        motor_drive_inst.timeOut[1] = 0;
        break;
    default:
        break;
    }
}
MOTORCONTROL_Typedef * getMotorInst(void)
{
	return &motor_drive_inst;
}
