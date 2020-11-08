#include "aqm_motor_driver.h"
#include "cmsis_os.h"
#include "Motor_Control.h"
MOTOR_STATUS motor_status;
MOTOR_TOALE_Pulse_Number motor_toale_trip_pulse_number;
MOTOR_TOALE_TRIP motor_toale_trip;
MOTORSTUDY_STATUS motorstudy_status;
MOTORWORK_TYPE motorwork_type;
MOTOR_PARAMETER motor_parameter= {
    1700,
    1800,
    1,
    0,
    0,
    5000,
    5000,
    50000,
    40000,
    6000,
    0,
    0,
    1,
};
MOTOR_Frequency motor_frequency = {
    0,
    0,
};
MOTOR_REALSPEED motor_speed = {
    0,
    0,
};
SET_PID_STRUCY pid_struct = {
    kp,
    ki,
    kd,
};
READ_PID_STRUCY right_read_pid_struct = {
    0,
    0,
    0,
};
READ_PID_STRUCY left_read_pid_struct = {
    0,
    0,
    0,
};
void Init_NewMotor_Communication(void)
{
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart6,ModbusRTU.ReceiveData,Mb_RECE__MAX_LENTH);
}
void Set_LeftRight_Motor_RatedCurrent(void)
{
    TYPE_MbComunication Right_RatedCurrent_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x006a,1,(uint8_t*)&motor_parameter.RatedCurrent,10};
    TYPE_MbComunication Left_RatedCurrent_Parameter = {Mb_WRITE_SINGLE,Left_Motor_Address, 0x006a,1,(uint8_t*)&motor_parameter.RatedCurrent,10};
    Mb_Poll(Right_RatedCurrent_Parameter);
    Mb_Working();
    HAL_Delay(10);
    Mb_Poll(Left_RatedCurrent_Parameter);
    Mb_Working();
    HAL_Delay(10);
}
void Set_LeftRight_Motor_MaxLoadCurrent(void)
{
    TYPE_MbComunication Right_MaxLoadCurrent_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x006b,1,(uint8_t*)&motor_parameter.MaxLoadCurrent,10};
    TYPE_MbComunication Left_MaxLoadCurrent_Parameter = {Mb_WRITE_SINGLE,Left_Motor_Address,0x006b,1,(uint8_t*)&motor_parameter.MaxLoadCurrent,10};
    Mb_Poll(Right_MaxLoadCurrent_Parameter);
    Mb_Working();
    HAL_Delay(10);
    Mb_Poll(Left_MaxLoadCurrent_Parameter);
    Mb_Working();
    HAL_Delay(10);
}
void LeftRightMotor_Study(void)
{
    TYPE_MbComunication Right_Study_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x00e1,1,(uint8_t*)&motor_parameter.MotorStudy,10};
    TYPE_MbComunication Left_Study_Parameter = {Mb_WRITE_SINGLE,Left_Motor_Address, 0x00e1,1,(uint8_t*)&motor_parameter.MotorStudy,10};
    Mb_Poll(Right_Study_Parameter);
    Mb_Working();
    HAL_Delay(10);
    Mb_Poll(Left_Study_Parameter);
    Mb_Working();
    HAL_Delay(10);
}
void Set_LeftRightMotor_Parameter()
{
    TYPE_MbComunication Right_Motor_Parameter= {Mb_WRITE_MULTIPLE,Right_Motor_Address,0x0060,8,(uint8_t*)&motor_parameter.SpeedUp_Buffer_Time,10};
    TYPE_MbComunication Left_Motor_Parameter = {Mb_WRITE_MULTIPLE,Left_Motor_Address, 0x0060,8,(uint8_t*)&motor_parameter.SpeedUp_Buffer_Time,10};
    Mb_Poll(Right_Motor_Parameter);
    Mb_Working();
    HAL_Delay(100);
    Mb_Poll(Left_Motor_Parameter);
    Mb_Working();
    HAL_Delay(100);
}
void Set_Motor_Speed(float speed,uint8_t motor_type)
{
    static int16_t rpm_right;
    static int16_t rpm_left;

    if(motor_type == Left_Motor)
    {
//		motor_speed.RightMotor_RealSpeed = (float)(((((float)motor_frequency.RightMotor_frequency*0.1f)*20.0f)/(Number_of_motor_pole*Reduction_ratio))/60.0f*DIAMETER_WHEEL * 3.1415f);

        rpm_left =  (speed/(motor_parameter_.DIAMETER_WHEEL_ * 3.1415f) * 60.0f) * (motor_parameter_.Number_of_motor_pole_*motor_parameter_.Reduction_ratio_) / 0.1f /20.0f;

        if ((ChassisType_ == WHEEL_CHAIR1)&&(MotorType_ == MOTOR_ORIGINAL))
        {
            rpm_left = -rpm_left;
        }

        TYPE_MbComunication Left_Parameter= {Mb_WRITE_SINGLE,0x01,0x0043,1,(uint8_t*)&rpm_left,10};
        Mb_Poll(Left_Parameter);
        Mb_Working();
    }
    if(motor_type == Right_Motor)
    {
        rpm_right = -(speed/(motor_parameter_.DIAMETER_WHEEL_ * 3.1415f) * 60.0f) * (motor_parameter_.Number_of_motor_pole_*motor_parameter_.Reduction_ratio_) / 0.1f /20.0f;;
        TYPE_MbComunication Right_Parameter= {Mb_WRITE_SINGLE,0x02,0x0043,1,(uint8_t*)&rpm_right,10};
        Mb_Poll(Right_Parameter);
        Mb_Working();
    }
    if(motor_type == LeftRight_Motor)
    {
        rpm_right = -(speed/(motor_parameter_.DIAMETER_WHEEL_ * 3.1415f) * 60.0f) * (motor_parameter_.Number_of_motor_pole_*motor_parameter_.Reduction_ratio_) / 0.1f /20.0f;
        rpm_left  = -rpm_right;
        if((ChassisType_ == WHEEL_CHAIR1)&&(MotorType_ == MOTOR_ORIGINAL))
        {
            rpm_left = -rpm_left;
        }
        TYPE_MbComunication Right_Parameter= {Mb_WRITE_SINGLE,0x01,0x0043,1,(uint8_t*)&rpm_left,10};
        TYPE_MbComunication Left_Parameter= {Mb_WRITE_SINGLE,0x02,0x0043,1,(uint8_t*)&rpm_right,10};
        Mb_Poll(Right_Parameter);
        Mb_Poll(Left_Parameter);
        Mb_Working();
    }
}
void Set_Motor_Stop(uint16_t Stop_Type,uint8_t motor_type)
{
    static int16_t right_Stop_Type;
    static int16_t left_Stop_Type;
    if(motor_type == Left_Motor)
    {
        left_Stop_Type = Stop_Type;
        TYPE_MbComunication LeftStop_Parameter= {Mb_WRITE_SINGLE,Left_Motor_Address,0x0040,1,(uint8_t*)&left_Stop_Type,10};
        Mb_Poll(LeftStop_Parameter);
        Mb_Working();
    }
    if(motor_type == Right_Motor)
    {
        right_Stop_Type = Stop_Type;
        TYPE_MbComunication RightStop_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x0040,1,(uint8_t*)&right_Stop_Type,10};
        Mb_Poll(RightStop_Parameter);
        Mb_Working();
    }
    if(motor_type == LeftRight_Motor)
    {
        left_Stop_Type = Stop_Type;
        right_Stop_Type = Stop_Type;
        TYPE_MbComunication LeftStop_Parameter= {Mb_WRITE_SINGLE,Left_Motor_Address,0x0040,1,(uint8_t*)&left_Stop_Type,10};
        TYPE_MbComunication RightStop_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x0040,1,(uint8_t*)&right_Stop_Type,10};
        Mb_Poll(LeftStop_Parameter);
        Mb_Poll(RightStop_Parameter);
        Mb_Working();
    }

}
void Set_Motor_ControlType(void)
{
    TYPE_MbComunication Right_ControlType_Parameter= {Mb_WRITE_SINGLE,Right_Motor_Address,0x0070,1,(uint8_t*)&motor_parameter.Speed_Closed_Loop,10};
    TYPE_MbComunication Left_ControlType_Parameter = {Mb_WRITE_SINGLE,Left_Motor_Address, 0x0070,1,(uint8_t*)&motor_parameter.Speed_Closed_Loop,10};

    Mb_Poll(Right_ControlType_Parameter);
    Mb_Working();
    HAL_Delay(10);

    Mb_Poll(Left_ControlType_Parameter);
    Mb_Working();
    HAL_Delay(10);
}
void Set_Motor_PIDParameter(void)
{

    uint8_t *Modbus_pHoldReg = ((uint8_t*)(&pid_struct.Kp));
    uint8_t *Modbus_iHoldReg = ((uint8_t*)(&pid_struct.Ki));
    uint8_t *Modbus_dHoldReg = ((uint8_t*)(&pid_struct.Kd));
    uint16_t _buffer[6];

    _buffer[1] = *(uint16_t*)Modbus_pHoldReg;
    _buffer[0] = *((uint16_t*)Modbus_pHoldReg+1);
    _buffer[3] = *((uint16_t*)Modbus_iHoldReg);
    _buffer[2] = *((uint16_t*)Modbus_iHoldReg+1);
    _buffer[5] = *((uint16_t*)Modbus_dHoldReg);
    _buffer[4] = *((uint16_t*)Modbus_dHoldReg+1);

    TYPE_MbComunication Right_PID_Parameter= {Mb_WRITE_MULTIPLE,Right_Motor_Address,0x00c0,6,(uint8_t*)_buffer,10};
    TYPE_MbComunication Left_PID_Parameter = {Mb_WRITE_MULTIPLE,Left_Motor_Address, 0x00c0,6,(uint8_t*)_buffer,10};
    Mb_Poll(Right_PID_Parameter);

    Mb_Working();
    HAL_Delay(100);
    Mb_Poll(Left_PID_Parameter);

    Mb_Working();
    HAL_Delay(100);
}


void Read_MotorStudy_Status(void)
{
    TYPE_MbComunication ReadRightMotorStudy_Status= {Mb_READ,Right_Motor_Address,0x0075,1,(uint8_t*)&motorstudy_status.RightMotor_StudyStatus,10};
    Mb_Poll(ReadRightMotorStudy_Status);
    Mb_Working();
    HAL_Delay(10);

    TYPE_MbComunication ReadLeftMotorStudy_Status= {Mb_READ,Left_Motor_Address,0x0075,1,(uint8_t*)&motorstudy_status.LeftMotor_StudyStatus,10};
    Mb_Poll(ReadLeftMotorStudy_Status);
    Mb_Working();
    HAL_Delay(10);
}

void Read_PID_Parameter(void)
{
    static uint16_t pid_buffer[6];
    uint16_t *read_p = (uint16_t*)&left_read_pid_struct.Kp;
    uint16_t *read_i = (uint16_t*)&left_read_pid_struct.Ki;
    uint16_t *read_d = (uint16_t*)&left_read_pid_struct.Kd;

    for(int i = 0; i<3; i++)
    {
        TYPE_MbComunication ReadLeft_PID= {Mb_READ,Left_Motor_Address,0x00c0,6,(uint8_t*)pid_buffer,10};
        Mb_Poll(ReadLeft_PID);
        Mb_Working();
        HAL_Delay(1);

        *read_p 		= *(&pid_buffer[1]);
        *(read_p+1) = *(&pid_buffer[0]);
        *read_i 		= *(&pid_buffer[3]);
        *(read_i+1) = *(&pid_buffer[2]);
        *read_d 		= *(&pid_buffer[5]);
        *(read_d+1) = *(&pid_buffer[4]);
    }
    read_p = (uint16_t*)&right_read_pid_struct.Kp;
    read_i = (uint16_t*)&right_read_pid_struct.Ki;
    read_d = (uint16_t*)&right_read_pid_struct.Kd;

    TYPE_MbComunication ReadRight_PID= {Mb_READ,Right_Motor_Address,0x00c0,6,(uint8_t*)pid_buffer,10};
    Mb_Poll(ReadRight_PID);
    Mb_Working();
    HAL_Delay(1);

    *read_p 		= *(&pid_buffer[1]);
    *(read_p+1) = *(&pid_buffer[0]);
    *read_i 		= *(&pid_buffer[3]);
    *(read_i+1) = *(&pid_buffer[2]);
    *read_d 		= *(&pid_buffer[5]);
    *(read_d+1) = *(&pid_buffer[4]);
}
void Read_MotorWorkType(void)
{
    TYPE_MbComunication ReadRight_WorkType= {Mb_READ,Right_Motor_Address,0x0070,1,(uint8_t*)&motorwork_type.RightMotor_WorkType,10};
    TYPE_MbComunication ReadLeft_WorkType = {Mb_READ,Left_Motor_Address, 0x0070,1,(uint8_t*)&motorwork_type.LeftMotor_WorkType,10};
    Mb_Poll(ReadRight_WorkType);
    Mb_Working();
    HAL_Delay(10);

    Mb_Poll(ReadLeft_WorkType);
    Mb_Working();
    HAL_Delay(10);
}

//void Read_MotorRealSpeed(MOTOR_REALSPEED *const speed)
void Read_MotorRealSpeed(void)
{
    TYPE_MbComunication ReadRight_RealSpeed= {Mb_READ,Right_Motor_Address,0x0022,1,(uint8_t*)&motor_frequency.RightMotor_frequency,10};
    TYPE_MbComunication ReadLeft_RealSpeed = {Mb_READ,Left_Motor_Address, 0x0022,1,(uint8_t*)&motor_frequency.LeftMotor_frequency,10};
    Mb_Poll(ReadRight_RealSpeed);
    Mb_Working();
    osDelay(1);
    Mb_Poll(ReadLeft_RealSpeed);
    Mb_Working();
    osDelay(1);
    motor_speed.RightMotor_RealSpeed = -(float)(((((float)motor_frequency.RightMotor_frequency*0.1f)*20.0f)/(motor_parameter_.Number_of_motor_pole_*motor_parameter_.Reduction_ratio_)) /60.0f*motor_parameter_.DIAMETER_WHEEL_ * 3.1415f);
    motor_speed.LeftMotor_RealSpeed = (float)(((((float)motor_frequency.LeftMotor_frequency*0.1f) *20.0f)/(motor_parameter_.Number_of_motor_pole_*motor_parameter_.Reduction_ratio_))/60.0f*motor_parameter_.DIAMETER_WHEEL_ * 3.1415f);
    if ((ChassisType_ == WHEEL_CHAIR1)&&(MotorType_ == MOTOR_ORIGINAL))
    {
        motor_speed.RightMotor_RealSpeed = -motor_speed.RightMotor_RealSpeed;
    }
}
void Read_MotorStatus(void)
{
    TYPE_MbComunication ReadRightMotor_Status= {Mb_READ,Right_Motor_Address,0x0033,1,(uint8_t*)&motor_status.RightMotor_Status,10};
    Mb_Poll(ReadRightMotor_Status);
    Mb_Working();


    TYPE_MbComunication ReadLeftMotor_Status= {Mb_READ,Left_Motor_Address,0x0033,1,(uint8_t*)&motor_status.LeftMotor_Status,10};
    Mb_Poll(ReadLeftMotor_Status);
    Mb_Working();

}
void Read_Total_Trip(void)
{
    static int16_t  left_motor_trip[2];
    static int16_t  right_motor_trip[2];
    uint16_t *read_trip = (uint16_t*)&motor_toale_trip_pulse_number.LeftMotor_Toale_Pulse_Number;
    TYPE_MbComunication ReadLeft_Trip= {Mb_READ,Left_Motor_Address,0x0024,2,(uint8_t*)left_motor_trip,10};
    Mb_Poll(ReadLeft_Trip);
    Mb_Working();


    *(read_trip+1) =  *(&left_motor_trip[0]);
    *(read_trip)   =  *(&left_motor_trip[1]);


    read_trip = (uint16_t*)&motor_toale_trip_pulse_number.RightMotor_Toale_Pulse_Number;
    TYPE_MbComunication ReadRight_Trip= {Mb_READ,Right_Motor_Address,0x0024,2,(uint8_t*)right_motor_trip,10};
    Mb_Poll(ReadRight_Trip);
    Mb_Working();


    *(read_trip+1)=  *(&right_motor_trip[0]);
    *(read_trip)  =  *(&right_motor_trip[1]);
    motor_toale_trip.RightMotor_Toale_Trip = -(float)(((float)motor_toale_trip_pulse_number.RightMotor_Toale_Pulse_Number/544.0f)*motor_parameter_.DIAMETER_WHEEL_ * 3.1415f);
    motor_toale_trip.LeftMotor_Toale_Trip  = (float)(((float)motor_toale_trip_pulse_number.LeftMotor_Toale_Pulse_Number /544.0f)*motor_parameter_.DIAMETER_WHEEL_ * 3.1415f);
}
void AQMDBL_scramFlag(unsigned char *pFlag)
{
    static unsigned char flag_buf;
    if(ModbusRTU.ErrorStatus == Mb_ERR_TIMEOUT)
    {
        flag_buf++;
        if(flag_buf>50)
            flag_buf = 50;
    } else
    {
        flag_buf--;
        if(flag_buf>50)
            flag_buf = 0;
    }

    if(flag_buf>25)
    {
        *pFlag = 1;
    } else
    {
        *pFlag = 0;
    }
}
