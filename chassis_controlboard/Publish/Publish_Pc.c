#include "Publish_Pc.h"
#include "OD.h"
#include "string.h"
#include "Clock.h"
#include "sys_monitor.h"
#include "imu.h"
#include "odom.h"
#include "motor_driver.h"
#include "Publish_powerboard.h"
#include "control_debug.h"
#include "Ultrasonic_New.h"
#include "chassis_config.h"
#include "chassis_fsm.h"
#include "sys_queue.h"
#include "SD_Log_App.h"
#include "Pub_ControlInfo.h"

#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif


// 处理状态上报事件
uint8_t handleFsmPubEvt(uint16_t *pmsg)
{
    uint16_t msg = CHASSIS_STATE_BUTTOM;//FSM_PUB_DISABLE;

    if(pmsg == NULL)
    {
        return FSM_PUB_DISABLE;
    }

    if(osEventMessage == pullFsmPubEvt(&msg))
    {
        *pmsg = msg;

        return FSM_PUB_ENABLE;
    }

    return FSM_PUB_DISABLE;
}

void publishFsmState(void)
{
    uint16_t msg = CHASSIS_STATE_BUTTOM;

    if(handleFsmPubEvt(&msg) == FSM_PUB_ENABLE)
    {
        if(msg > CHASSIS_STATE_BUTTOM) return;
        
        State_Switch_Pub_Buffer_frame.sAck.resend_num = 0;
        State_Switch_Pub_Buffer_frame.sAck.send_ack = 0;
        State_Switch_Pub_Buffer.state = msg;
        publishFrameData(TX2_ID, STATE_SWITCH_UP_ID, (uint8_t*)&State_Switch_Pub_Buffer, sizeof(State_Switch_Pub_Buffer));
        
				GR_LOG_INFO("fsm sending curState = %d %s", msg,logFsmState[msg]);
    }
}

void publishChassisEvent(void)
{
    uint16_t evt = CHASSIS_BUTTOM_EVT;

    if(osEventMessage != pullChassisEvt(&evt))
    {
        return;
    }

    if(evt < CHASSIS_BUTTOM_EVT)
    {
        Event_Pub_Buffer.state = evt;
        publishFrameData(TX2_ID,EVENT_UP_ID, (unsigned char *)&Event_Pub_Buffer, sizeof(Event_Pub_Buffer));
    }
}


void BatteryPubHandle(void)
{
    static unsigned long int t = 0;

    getSystemTimeStamp(t++, &Battery_Pub_Buffer.time);
    if(getBMSCommunicationState() == 0)
    {
        memset(&Bms_Info, 0x00, sizeof(Bms_Info));
    }
    Battery_Pub_Buffer.voltage = Bms_Info.total_volatage;
    Battery_Pub_Buffer.current = Bms_Info.current;
    Battery_Pub_Buffer.temperature = ((Bms_Info.temperature1 > Bms_Info.temperature2)? Bms_Info.temperature1 : Bms_Info.temperature2);
    Battery_Pub_Buffer.charge = (Bms_Info.status & 0x01);
    Battery_Pub_Buffer.capacity = Bms_Info.RemainingCapacity;
    Battery_Pub_Buffer.design_capacity = Bms_Info.FullChargeCapacity;
    Battery_Pub_Buffer.percentage = Bms_Info.RelativeStateOfCharge;

    publishFrameData(TX2_ID,BATTERY_INFO_ID, (unsigned char *)&Battery_Pub_Buffer, sizeof(Battery_InfoTypeDef));
}

void MonitorPubHandle(void)
{
    static System_State_TypeDef sys_state;
    static unsigned long int t = 0;

    if(getTX2CommunicationState() == 0)
    {
        return;
    }

    getSystemMonitor(&sys_state);
    ClearWatchdogStateMonitor();

    getSystemTimeStamp(t++, &Monitor_Pub_Buffer.time);

    memcpy(&Monitor_Pub_Buffer.stack_sensor, 
          &sys_state.stack.stack_sensor, sizeof(System_State_TypeDef));

    publishFrameData(TX2_ID,MONITOR_INFO_ID,
        (unsigned char *)&Monitor_Pub_Buffer, sizeof(Monitor_InfoTypeDef));
}

#ifdef __ALARM_USER
/*************************************************
Function: 			AlarmPubHandle
Description:		发送告警信息至平台,每1s发一次
*************************************************/
void AlarmPubHandle(void)
{
  static unsigned long int t = 0; 				
  // 检测通信是否正常
  if(getTX2CommunicationState() == 0)
  {
    return;
  }
  //添加时间戳
  getSystemTimeStamp(t++, &g_sAlarmPub.sA.time);

  //发送告警至平台 tx2
  publishFrameData(TX2_ID,ALARM_PUB_ID,g_sAlarmPub.buff,sizeof(unPUB_ALARM));

}
/*************************************************
Function: 			AlarmPubChange
Description:		发送告警信息至平台,每1s发一次，有变化立即发
*************************************************/
void AlarmPubChange(void)
{
    if (alarmMonitor())
    {
        AlarmPubHandle();
    }
}

#endif // __ALARM_USER

#if 0
void ChassisEventPubHanlder(void)
{
    Event_Pub_Buffer.state = state_machine.event;
    publishFrameData(TX2_ID,EVENT_UP_ID, (unsigned char *)&Event_Pub_Buffer, sizeof(Event_Pub_Buffer));
}


void UltrasonicPubHandle(void)
{
    static unsigned long int t = 0;
    static Ultrasonic_TypeDef ultrasonic_data;

    getSystemTimeStamp(t++, &Ultrasonic_Pub_Buffer.time);
//	Driver_Ultrasonic_Update(&ultrasonic_data);

    Ultrasonic_Pub_Buffer.count = 14;
    for(int i = 0; i < Ultrasonic_Pub_Buffer.count; i++)
    {
        Ultrasonic_Pub_Buffer.distance[i] = ultrasonic_data.distance[i];
    }

    publishFrameData(TX2_ID,ULTRASONIC_INFO_ID, (unsigned char *)&Ultrasonic_Pub_Buffer, sizeof(Ultrasonic_InfoTypeDef));
}
#endif

void LogUpPubHandle(char* info, unsigned char len)
{
    static unsigned long int t = 0;
    //unsigned short int cnt;

    memset(&Log_Up_Pub_Buffer, 0x00, sizeof(Log_UpTypeDef));

    getSystemTimeStamp(t++, &Log_Up_Pub_Buffer.time);
    Log_Up_Pub_Buffer.len = len;
    memcpy(&Log_Up_Pub_Buffer.info, info, len);
    publishFrameData(TX2_ID,LOG_UP_ID, (unsigned char *)&Log_Up_Pub_Buffer, sizeof(Log_UpTypeDef));
}

#define ACCEL_FACTOR                     -0.000598f  // 2.0 * -9.8 / 32768
#define GYRO_FACTOR                       0.000133f  // pi / (131 * 180)


void ImuPubHandle(void)
{
    static ImuPara_TypeDef ros_imu;
    static unsigned long int t = 0;

    getImuData(&ros_imu);

    getSystemTimeStamp(t++, &Imu_Pub_Buffer.time);

    //imu的结构体其实已经计算了真实的角速度和加速度
    Imu_Pub_Buffer.linear_x = ros_imu.Real_ACC[0];
    Imu_Pub_Buffer.linear_y = ros_imu.Real_ACC[1];
    Imu_Pub_Buffer.linear_z = ros_imu.Real_ACC[2];

    Imu_Pub_Buffer.angular_x = ros_imu.Real_Gyro[0];
    Imu_Pub_Buffer.angular_y = ros_imu.Real_Gyro[1];
    Imu_Pub_Buffer.angular_z = ros_imu.Real_Gyro[2];

    Imu_Pub_Buffer.orientation_w = ros_imu.orientation[0];
    Imu_Pub_Buffer.orientation_x = ros_imu.orientation[1];
    Imu_Pub_Buffer.orientation_y = ros_imu.orientation[2];
    Imu_Pub_Buffer.orientation_z = ros_imu.orientation[3];

    publishFrameData(TX2_ID,IMU_INFO_ID, (unsigned char *)&Imu_Pub_Buffer, sizeof(Imu_Pub_Buffer));
}

void OdomPubHandle(void)
{
    static Odom_TypeDef ros_odomData;
    static ImuPara_TypeDef ros_imu;
    static MOTORCONTROL_Typedef motor_arg;
    static unsigned long int t = 0;

    getOdomData(&ros_odomData);
    getImuData(&ros_imu);
    updateMotorData(&motor_arg);
    getSystemTimeStamp(t++, &Odom_Pub_Buffer.time);

    Odom_Pub_Buffer.position_x = ros_odomData.odom_pose[0];
    Odom_Pub_Buffer.position_y = ros_odomData.odom_pose[1];
    Odom_Pub_Buffer.position_z = 0;

    Odom_Pub_Buffer.orientation_w = ros_imu.orientation[0];
    Odom_Pub_Buffer.orientation_x = ros_imu.orientation[1];
    Odom_Pub_Buffer.orientation_y = ros_imu.orientation[2];
    Odom_Pub_Buffer.orientation_z = ros_imu.orientation[3];

    //	Odom_Pub_Buffer.angular_z = ros_odomData.odom_vel[2];同事反应没有上报角速度，故更改
    Odom_Pub_Buffer.angular_z = ros_imu.Real_Gyro[2];

    //Odom_Pub_Buffer.linear_x = ros_odomData.odom_vel[0] * (motor_arg.real_speed[0]+motor_arg.real_speed[1])/2;
    //由于上位机需要关于车体坐标系的线速度，故未按原意上传2019/3/4
    Odom_Pub_Buffer.linear_x = (motor_arg.real_speed[0] + motor_arg.real_speed[1])/2;
    Odom_Pub_Buffer.linear_y = ros_odomData.odom_vel[1] * (motor_arg.real_speed[0]+motor_arg.real_speed[1])/2;

    //前端检测速度为负作为倒车判断，由于在刹车停止时会产生一个极小的后退，所以会误报后退。现滤掉这个极小值
    if(fabs(Odom_Pub_Buffer.linear_x)<0.07f)
    {
        Odom_Pub_Buffer.linear_x = 0.0f;
    }

    publishFrameData(TX2_ID,ODOM_INFO_ID, (unsigned char *)&Odom_Pub_Buffer, sizeof(Odom_InfoTypeDef));
}

void ChassisSpeedInfoPubHandle(void)
{
    static MOTORCONTROL_Typedef motor_arg;

    updateMotorData(&motor_arg);

    Chassis_Speed_Buffer.linear_speed  = (motor_arg.real_speed[0]+motor_arg.real_speed[1]) / 2;

    publishFrameData(TX2_ID,CHASSIS_SPEED_ID, (unsigned char *)&Chassis_Speed_Buffer, sizeof(Chassis_SpeedTypeDef));
}

void ChassisMileageInfoPubHandle(void)
{
    static Odom_TypeDef pub_odom_info;

    getOdomData(&pub_odom_info);

    Chassis_Mileage_Buffer.mileage = (pub_odom_info.mileage[0] + pub_odom_info.mileage[1])/2;
    Chassis_Mileage_Buffer.total_mileage = pub_odom_info.total_mileage;
    publishFrameData(TX2_ID,CHASSIS_MILEAGE_ID, (unsigned char *)&Chassis_Mileage_Buffer, sizeof(Chassis_MileageTypeDef));
}

void Tx2PoweroffRequestPubHandle(void)
{
    Tx2_Poweroff_Buffer.cmd = 1;
    publishFrameData(TX2_ID,TX2_POWEROFF_ID, (unsigned char *)&Tx2_Poweroff_Buffer, sizeof(Tx2_PoweroffTypeDef));
}

void UltrasonicSampleCmdPubHandle(void)
{
    unsigned char len;

    len = Ultrasonic_Down_Sub_Buffer.len;
    if(len > 128)
        return;

    Ultrasonic_Sample_Send_Buffer.len = len;
    memcpy(Ultrasonic_Sample_Send_Buffer.info, Ultrasonic_Down_Sub_Buffer.info, len);
    publishFrameData(ULT_ID,PUB_ULT_SAMPLE_SEND_ID, (unsigned char *)&Ultrasonic_Sample_Send_Buffer, len + 2);
}

void UltrasonicPubTestHandle(unsigned char *pData, unsigned char cnt)
{
    static unsigned long int t = 0;
    unsigned char num, i;

    getSystemTimeStamp(t++, &Ultrasonic_Pub_Buffer.time);

    Ultrasonic_Pub_Buffer.count = cnt;

    for(i = 0; i < cnt; i++)
    {
        num = pData[i*3];
        if((num <= 12) && (num > 0))
        {
            Ultrasonic_Pub_Buffer.distance[num - 1] = (pData[(i*3) + 1] << 8) + pData[(i*3) + 2];
        }
    }

    publishFrameData(TX2_ID, ULTRASONIC_INFO_ID, (unsigned char *)&Ultrasonic_Pub_Buffer, sizeof(Ultrasonic_InfoTypeDef));
}
void con_to_tx2_framepub(uint8_t save_flag)
{
    static unsigned char data[sizeof(STATISTICS_CONTOTX2_MSG)];

    statistics_contotx2_msg.save_flag = save_flag;
    statistics_contotx2_msg.statistics_snd_42++;
    statistics_contotx2_msg.frame_total++;
    if(save_flag == 0)
    {
        statistics_tx2tocon_msg.statistics_snd_32++;
        statistics_tx2tocon_msg.frame_total++;
    }
    memcpy(&data, &statistics_contotx2_msg, sizeof(STATISTICS_CONTOTX2_MSG));
    publishFrameData(TX2_ID,CON_UP_SENDTOALE_ID, data, sizeof(STATISTICS_CONTOTX2_MSG));
}
void tx2_to_con_framepub(uint8_t save_flag)
{
    static unsigned char data[sizeof(STATISTICS_TX2toCON_msg)];

    statistics_tx2tocon_msg.save_flag = save_flag;
    statistics_contotx2_msg.statistics_rcv_43++;
    statistics_contotx2_msg.frame_total++;
    if(save_flag == 0)
    {
        statistics_tx2tocon_msg.statistics_rcv_33++;
        statistics_tx2tocon_msg.frame_total++;
    }
    memcpy(&data, &statistics_tx2tocon_msg, sizeof(STATISTICS_TX2toCON_msg));
    publishFrameData(TX2_ID,CON_UP_RECIVTOALE_ID, data, sizeof(STATISTICS_TX2toCON_msg));
}


void infoPublishHanlder(void)
{
    uint32_t t;
    static uint32_t tTime[12] = {0};

//    getUltrasonicChannelState();

    if(getTX2CommunicationState() == 0)
    {
        return;
    }

    publishFsmState();
    publishChassisEvent();
    t = getSystemTick();
    if (t - tTime[0] >= (1000 / IMU_PUBLISH_FREQ))
    {
        ImuPubHandle();
        tTime[0] = t;
    }

    t = getSystemTick();
    if (t - tTime[1] >= (1000 / ODOM_PUBLISH_FREQ))
    {
        OdomPubHandle();
        tTime[1] = t;
    }

    t = getSystemTick();
    if (t - tTime[2] >= (1000 / BATTERY_PUBLISH_FREQ))
    {
        BatteryPubHandle();
        tTime[2] = t;
    }

    t = getSystemTick();
    if (t - tTime[3] >= (1000 / ULTRASONIC_PUBLISH_FREQ))
    {
        pubUltrasonicSampleData();
        tTime[3] = t;
    }

    t = getSystemTick();
    if (t - tTime[4] >= (1000 / MONITOR_PUBLISH_FREQ))
    {
        MonitorPubHandle();
        tTime[4] = t;
    }

    t = getSystemTick();
    if (t - tTime[5] >= (1000 / CONTROL_DEBUG_PUBLISH_FREQ))
    {
        publishControlDebugInfo();
        tTime[5] = t;
    }

    t = getSystemTick();
    if (t - tTime[6] >= (1000 / CHASSIS_SPEED_PUBLISH_FREQ))
    {
        ChassisSpeedInfoPubHandle();
        tTime[6] = t;
    }
		
    t = getSystemTick();
    if (t - tTime[7] >= (1000 / CHASSIS_SPEED_PUBLISH_FREQ))
    {
        ChassisMileageInfoPubHandle();
        tTime[7] = t;
    }

    t = getSystemTick();
    if (t - tTime[8] >= (1000 / 1))  
    {
        HAL_GPIO_TogglePin(GPIOG, LED_RED_Pin);
#ifdef __ALARM_USER
        //定时一秒上一次
        AlarmPubHandle();
#endif
        tTime[8] = t;
    }

    t = getSystemTick();
    if (t - tTime[9] >= (1000 / CHASSIS_ALARM_CHANGE_FREQ))
    {
        AlarmPubChange();
        tTime[9] = t;
    }

		t = getSystemTick();
    if (t - tTime[10] >= (1000 / CONTROL_INFO_PUBLISH_FREQ))
    {
        pub_controlInfo();
        tTime[10] += 1000 / CONTROL_INFO_PUBLISH_FREQ;//精确的20Hz wqw 2019/7/18
    }
		
		t = getSystemTick();
    if (t - tTime[11] >= (1000 / ULTRASONIC_CHECK_FREQ))
    {
        getUltrasonicChannelState();
        tTime[11] = t;
    }
}
