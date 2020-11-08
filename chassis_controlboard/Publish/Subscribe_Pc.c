#include "Subscribe_Pc.h"
#include "OD.h"
#include <string.h>
#include "log_app.h"
#include "Publish_Pc.h"
#include "Clock.h"
#include "Ultrasonic_New.h"
#include "Frame_statistical.h"
#include "chassis_fsm.h"
#include "sys_queue.h"
#include "SD_Log_App.h"
#include "joystick.h"
#include "sys_monitor.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif


void StateSwitchSubCallBack(void)
{
    uint8_t state_cmd;

	  MainBoard_RecivceFrame_Statistical(STATE_SWITCH_DOWN_ID);//帧计数
    state_cmd = State_Switch_Sub_Buffer.state;

    if(state_cmd <= COMMOND_FAULT_REC)
    {				
      sendEvtToChassisFsm(state_cmd);
      GR_LOG_INFO("fsm cmd event get,curState=%d,receiving command = %d %s"
        , getChassisFsmCurState(), state_cmd,logFsmCmd[state_cmd]);
    }
    else if(state_cmd == COMMOND_FAULT_REPLY)
    {
        pushFsmPubEvt(getChassisFsmCurState());
    }
}
void LogDownSubCallBack(void)
{
    MainBoard_RecivceFrame_Statistical(LOG_DOWN_ID);
#ifdef __LOG
    Log_syscall_cmd(Log_Down_Sub_Buffer.cmd);
#endif
}

float goal_velocity_from_cmd[2] = {0.0, 0.0};

void UpdateRemoteControlData(float *data)
{
    data[0] = goal_velocity_from_cmd[0];
    data[1] = goal_velocity_from_cmd[1];
}
void ResetRemoteControlData()
{
    goal_velocity_from_cmd[0] = 0;
    goal_velocity_from_cmd[1] = 0;
}

void CmdVelbSubCallBack(void)
{
#ifdef __TP_USER
    TP_RETURN(eTP62_cmdvelTimeout);
#endif
    clearCmdVelTimeOut();
    MainBoard_RecivceFrame_Statistical(CMD_VEL_ID);
    goal_velocity_from_cmd[0] = Cmd_Vel_Sub_Buffer.linear_x;
    goal_velocity_from_cmd[1] = Cmd_Vel_Sub_Buffer.angular_z;
}
// [0]速度限制 [1]角度限制
float speed_limit[2][2] = {{-3.0,3.0},{-3.0,3.0}};

void ChassisSpeedLimitSubCallBack(void)
{
    MainBoard_RecivceFrame_Statistical(SPEED_LIMIT);
    if(Chassis_speed_limit.speed_limit_enable_flag == 1
       && (getJoyMode() == OFF) )
    {
			if((Chassis_speed_limit.min_linear_limit_speed <= Chassis_speed_limit.max_linear_limit_speed)
							&& (Chassis_speed_limit.min_angular_limit_speed <= Chassis_speed_limit.max_angular_limit_speed))
			{
					speed_limit[0][0] = Chassis_speed_limit.min_linear_limit_speed;
					speed_limit[0][1] = Chassis_speed_limit.max_linear_limit_speed;
					speed_limit[1][0] = Chassis_speed_limit.min_angular_limit_speed;
					speed_limit[1][1] = Chassis_speed_limit.max_angular_limit_speed;
			}
    }
    else
    {
        speed_limit[0][0] = -3.0;
        speed_limit[0][1] = 3.0;
        speed_limit[1][0] = -3.0;
        speed_limit[1][1] = 3.0;
    }

    if(speed_limit[0][0]>0)
    {
        speed_limit[0][0] = 0;
    }
    if(speed_limit[0][1]<0)
    {
        speed_limit[0][1] = 0;
    }
    if(speed_limit[1][0]>0)
    {
        speed_limit[1][0] = 0;
    }
    if(speed_limit[1][1]<0)
    {
        speed_limit[1][1] = 0;
    }
}

void GetSpeedLimitPara(float *pdata)
{
    pdata[0] = Chassis_speed_limit.speed_limit_enable_flag;
    pdata[1] = speed_limit[0][0];
    pdata[2] = speed_limit[0][1];
    pdata[3] = speed_limit[1][0];
    pdata[4] = speed_limit[1][1];
}

void Control_COF_CallBack(void)
{
    MainBoard_RecivceFrame_Statistical(CONTROL_PARA_ID);
}

static uint8_t power_ack = 0x00;

void Tx2PoweroffCallBack(void)
{
    MainBoard_RecivceFrame_Statistical(TX2_POWEROFF_ACK_ID);
    power_ack = Tx2_Poweroff_Ack_Buffer.cmd;
}

uint8_t Tx2PoweroffGetAck(void)
{
    return power_ack;
}

void Tx2PoweroffCleanAck(void)
{
    power_ack = 0x00;
}

void UltrasonicTX2CmdCallBack(void)
{
    MainBoard_RecivceFrame_Statistical(ULTRASONIC_DOWN_ID);
    UltrasonicSampleCmdPubHandle();
}

static unsigned short int ultrasonic_work_state = 0;


void UltrasonicSampleCmdCallBack(void)
{
    unsigned char len, data_len, group_num;
    static unsigned long int t = 0;

    len = Ultrasonic_Sample_Recv_Buffer.len;
    if((len > 128) || (len == 0))
        return;

    Ultrasonic_Sample_Recv_Buffer.len = 0;

//	if(getTX2CommunicationState() == 0)
//	{
//		return;
//	}

    if((Ultrasonic_Sample_Recv_Buffer.info[0] == 0x3A)
            && (Ultrasonic_Sample_Recv_Buffer.info[1] <= 0x0D)
            && (Ultrasonic_Sample_Recv_Buffer.info[2] <= 0x01))
    {
        if(Ultrasonic_Sample_Recv_Buffer.info[1] == 0x0C)
        {
            if(getTX2CommunicationState() == 0)
            {
                return;
            }

            data_len = Ultrasonic_Sample_Recv_Buffer.info[3];
            group_num = Ultrasonic_Sample_Recv_Buffer.info[4];
            if(group_num == 0)
                return;

            if(data_len == (group_num * 3) + 1)
            {
                receiveUltrasonicSampleData(group_num, &Ultrasonic_Sample_Recv_Buffer.info[5]);
            }
        }
        else if(Ultrasonic_Sample_Recv_Buffer.info[1] == 0x0D)
        {
						ultrasonic_work_state = (Ultrasonic_Sample_Recv_Buffer.info[4] << 8) | Ultrasonic_Sample_Recv_Buffer.info[5]; 
        }
        else
        {
            if(getTX2CommunicationState() == 0)
            {
                return;
            }

            getSystemTimeStamp(t++, &Ultrasonic_Up_Pub_Buffer.time);
            Ultrasonic_Up_Pub_Buffer.len = len;
            memcpy(Ultrasonic_Up_Pub_Buffer.info, Ultrasonic_Sample_Recv_Buffer.info, len);
            publishFrameData(TX2_ID,ULTRASONIC_UP_ID, (unsigned char *)&Ultrasonic_Up_Pub_Buffer, len + 12 + 2);
        }
    }
}

unsigned short int getUltrasonicModuleState(void)
{
    return ultrasonic_work_state;
}


void CON_SENDFRAMECallback(void)
{
    if(strcmp(Device_Send,"device_snd") == 0)
    {
        con_to_tx2_framepub(No_SAVE_FRAME);
    }
}
void CON_RECIVCEFRAMECallback(void)
{
    if(strcmp(Device_Recivce,"device_rcv") == 0)
    {
        tx2_to_con_framepub(No_SAVE_FRAME);
    }
}
void COM_DELAYFLAGCallback(void)
{
	unsigned char data;
	com_delay_pub = 1;
	data = com_delay_pub;
	
	publishFrameData(TX2_ID,PUB_COM_DELAYPUB_ID,&data,sizeof(data));
}
