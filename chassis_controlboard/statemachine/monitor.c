#include "monitor.h"
#include "outboard_sensor.h"
#include "sys_monitor.h"
#include "Subscribe_powerBoard.h"
#include "joystick.h"
#include "motor_driver.h"
#include "chassis_config.h"
#include "chassis_fsm.h"

//Sensor_State_TypeDef sys_state;

#if 0
void updateMotorStopData(uint8_t state)
{
//	state_machine.sensor_flag.emergency_stop_last_flag = state_machine.sensor_flag.emergency_stop_flag;
//	state_machine.sensor_flag.emergency_stop_flag = state;
}



void updateSensorState(void)
{
    static uint8_t joystick_num=0;

    System_State_TypeDef sys_state;
    MOTORCONTROL_Typedef motor_data;
    Bms_Buffer battery_state;
    Joystick_TypeDef joystick_data;

    updateSystemMonitor(&sys_state);
    updateMotorData(&motor_data);
    updateBmsProtocolData(&battery_state);
    updateJoystickData(&joystick_data);
//	updateMotorStopData(&joystick_data);

    sendSystemCheckEvtToChassisFsm();
    if(getTX2CommunicationState() == 1)
    {
        state_machine.sensor_flag.lastconnectFlag = state_machine.sensor_flag.connectFlag;
        state_machine.sensor_flag.connectFlag = 1;
    }
    else
    {
        state_machine.sensor_flag.lastconnectFlag = state_machine.sensor_flag.connectFlag;
        state_machine.sensor_flag.connectFlag = 0;
    }

    if(motor_data.scramFlag == 1)
    {
        state_machine.sensor_flag.emergency_stop_last_flag = state_machine.sensor_flag.emergency_stop_flag;
        state_machine.sensor_flag.emergency_stop_flag = 1;
    }
    else
    {
        state_machine.sensor_flag.emergency_stop_last_flag = state_machine.sensor_flag.emergency_stop_flag;
        state_machine.sensor_flag.emergency_stop_flag = 0;
    }
    if(fabs(motor_data.real_speed[0]) >= 0.1 || fabs(motor_data.real_speed[1]) >= 0.1)
    {
        state_machine.sensor_flag.stop_last_flag = state_machine.sensor_flag.stop_flag;
        state_machine.sensor_flag.stop_flag = 0;
    }
    else if(fabs(motor_data.real_speed[0]) <= 0.05 && fabs(motor_data.real_speed[1]) <= 0.05)
    {
        state_machine.sensor_flag.stop_last_flag = state_machine.sensor_flag.stop_flag;
        state_machine.sensor_flag.stop_flag = 1;
    }

    if(state_machine.sensor_flag.connectFlag == 1
            && state_machine.sensor_flag.lastconnectFlag == 0)
    {
        state_machine.init_connected_flag = 1;
    }
    if(battery_state.status == 1)
    {
        state_machine.sensor_flag.charging_last_flag = state_machine.sensor_flag.charging_flag;
        state_machine.sensor_flag.charging_flag = 1;
    }
    else
    {
        state_machine.sensor_flag.charging_last_flag = state_machine.sensor_flag.charging_flag;
        state_machine.sensor_flag.charging_flag = 0;
    }
#if 0//#ifndef __STATE_TEST
    // 正常使用时;若不上报故障下位机故障为则不需要该部分
    if(state_machine.sensor_flag.connectFlag == 0   //	|| sys_state.bms !=0  	|| state_machine.sensor_flag.emergency_stop_flag == 1
            || sys_state.joystick != 0
            || (sys_state.motor != 0)
            || sys_state.imu == 2) //
    {
        state_machine.sensor_flag.malfunction_last_flag = state_machine.sensor_flag.malfunction_flag;
        state_machine.sensor_flag.malfunction_flag = 1;
    }
    else
    {
        state_machine.sensor_flag.malfunction_last_flag = state_machine.sensor_flag.malfunction_flag;
        state_machine.sensor_flag.malfunction_flag = 0;
    }
#endif


    if(joystick_data.offset_val_x >0.2f || joystick_data.offset_val_x < -0.2f
            || joystick_data.offset_val_y >0.2f || joystick_data.offset_val_y < -0.1f)
    {
        joystick_num++;
        if(joystick_num>=10)  //Discriminate multiple times
        {
            joystick_num = 10;
            state_machine.sensor_flag.joystick_last_flag = state_machine.sensor_flag.joystick_flag;
            state_machine.sensor_flag.joystick_flag = 1;
//						sendJoystickEvtToChassisFsm();
        }
    }
    else
    {
        joystick_num = 0;
        state_machine.sensor_flag.joystick_last_flag = state_machine.sensor_flag.joystick_flag;
        state_machine.sensor_flag.joystick_flag = 0;
    }
}

void reportingEventHanlder(void)
{
    if(state_machine.sensor_flag.stop_flag == 0
            && state_machine.sensor_flag.stop_last_flag == 1)
    {
        state_machine.event = PARKING_END;
        state_machine.event_happen_flag = 1;
    }
    else if(state_machine.sensor_flag.stop_flag == 1
            && state_machine.sensor_flag.stop_last_flag == 0)
    {
        state_machine.event = PARKING_START;
        state_machine.event_happen_flag = 1;
    }

    if(state_machine.sensor_flag.emergency_stop_flag == 1
            && state_machine.sensor_flag.emergency_stop_last_flag == 0)
    {
        //state_machine.sensor_flag.emergency_stop_last_flag = state_machine.sensor_flag.emergency_stop_flag;
        state_machine.event = BREAK_PRESS;
        state_machine.event_happen_flag = 1;
    }
    else if(state_machine.sensor_flag.emergency_stop_flag == 0
            && state_machine.sensor_flag.emergency_stop_last_flag == 1)
    {
        //state_machine.sensor_flag.emergency_stop_last_flag = state_machine.sensor_flag.emergency_stop_flag;
        state_machine.event = BREAK_RECOVERY;
        state_machine.event_happen_flag = 1;
    }

    if(state_machine.sensor_flag.charging_flag == 1
            && state_machine.sensor_flag.charging_last_flag == 0)
    {
        state_machine.event = CHARGING_START;
        state_machine.event_happen_flag = 1;
    }
    else if(state_machine.sensor_flag.charging_flag == 0
            && state_machine.sensor_flag.charging_last_flag == 1)
    {
        state_machine.event = CHARGING_END;
        state_machine.event_happen_flag = 1;
    }

    if(state_machine.event_happen_flag == 1  && state_machine.sensor_flag.connectFlag == 1)
    {
        state_machine.event_happen_flag = 0;
        Send_ChassisEvent();
    }
}

//uint8_t getParkingState(void)
//{
//    return state_machine.sensor_flag.stop_flag;
////	return 1;
//}

#endif

