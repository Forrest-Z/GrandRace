#include "KEYA_Can.h"
#include "Clock.h"
#include "controlFilter.h"
#include "idea.h"
#include "alarm_com.h"
#include "fault_injector_wrapper.h"
#include "cmsis_os.h"
#include "SD_Log_App.h"

#define setErrCom_KEYA(type,errFlag) if(errFlag)	\
																	setAlarm(eA_DRIVER,type,ALARM_ERR);\
																else 	\
																	setAlarm(eA_DRIVER,type,ALARM_OK);

float speed_KEYA[2];  //读取编码器左右速度值 0左速度 1右速度
float distance_KEYA[2];
float elec_KEYA[2];							
float vlot_KEYA;															
unsigned char err_KEYA[2];
																

float numPloe_KEYA;
ExtremeFilterType speed_KEYA_Filter[2];


void SendCAN_KEYA(CAN_TxHeaderTypeDef *TxMessage_can,uint8_t TR_BUF_can[])
{
    uint32_t free_level = 0U;
    uint16_t outtime = 0;

//	if(EXSD_err==ERR_CAN_LOSE)
//	{
//		return;
//	}

    while(free_level==0) //一共有三个邮箱，只要有一个邮箱空闲就可以了。
    {
        free_level=HAL_CAN_GetTxMailboxesFreeLevel(&hcan2);

				if(free_level==0)
				{
          //以前的等待邮箱的方式会导致CPU占用，所以现在改为osDelay
					osDelay(1);  
					outtime++;
				}
				
        if (outtime > 3)
        {         
            return;
        }
    }
    if(HAL_CAN_AddTxMessage(&hcan2,TxMessage_can,
          TR_BUF_can,(uint32_t*)CAN_TX_MAILBOX0) != HAL_OK)
    {
        //error_send++;
    }

}
void init_KEYA(MOTORCONTROL_Typedef *motor_arg)
{
    speed_KEYA_Filter[0].max = ExtremeFilterMAX;
    speed_KEYA_Filter[1].max = ExtremeFilterMAX;

    speed_KEYA_Filter[0].k = ExtremeFilterK;
    speed_KEYA_Filter[1].k = ExtremeFilterK;

    speed_KEYA_Filter[0].inData = 0;
    speed_KEYA_Filter[1].inData = 0;
    for(int i = 0; i<ExtremeFilter; i++)
    {
        extremeFilter(&speed_KEYA_Filter[0]);
        extremeFilter(&speed_KEYA_Filter[1]);
    }

    numPloe_KEYA  = motor_arg->motor_parameter_Ptr->Number_of_motor_pole_;
    numPloe_KEYA /= motor_arg->motor_parameter_Ptr->Encoder_K;
}

/*************************************************
Function: 			readErr_KEYA
Description:		读取科亚错误编码 读取故障标志
Input:					none
Output: 				none
Return: 				none
*************************************************/
unsigned short readErr_KEYA(void)
{
    //发出CAN查询
    static uint8_t TR_BUF[4]= {0x40,0x12,0x21,0x00};
    unsigned short errBuf = 0;
   
    CAN_TxHeaderTypeDef TxMessage;
    TxMessage.DLC=4;
    TxMessage.StdId=0x601;
    TxMessage.IDE=CAN_ID_STD;//IDE位为标识符鿉择位（0，使用标准标识符＿1，使用扩展标识符＿
    TxMessage.RTR=CAN_RTR_DATA;//RTR位用于标识是否是远程
    SendCAN_KEYA(&TxMessage,TR_BUF);

    /*错误值编码
    ***************************/

#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP43_driver_left_err_overheat,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F1_KEYA);
    TP_MODIFY_VALUE(eTP44_driver_left_err_overvoltage,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F2_KEYA);
    TP_MODIFY_VALUE(eTP45_driver_left_err_undervoltage,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F3_KEYA);
    TP_MODIFY_VALUE(eTP46_driver_left_err_short_circuit,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F4_KEYA);
    TP_MODIFY_VALUE(eTP47_driver_left_err_emergency_stop,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F5_KEYA);
    TP_MODIFY_VALUE(eTP48_driver_left_err_sensor,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F6_KEYA);
    TP_MODIFY_VALUE(eTP49_driver_left_err_mos,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F7_KEYA);
    TP_MODIFY_VALUE(eTP50_driver_left_err_load_config,err_KEYA[eM_L],err_KEYA[eM_L]|ERR_F8_KEYA);
    
    TP_MODIFY_VALUE(eTP35_driver_right_err_overheat,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F1_KEYA);
    TP_MODIFY_VALUE(eTP36_driver_right_err_overvoltage,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F2_KEYA);
    TP_MODIFY_VALUE(eTP37_driver_right_err_undervoltage,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F3_KEYA);
    TP_MODIFY_VALUE(eTP38_driver_right_err_short_circuit,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F4_KEYA);
    TP_MODIFY_VALUE(eTP39_driver_right_err_emergency_stop,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F5_KEYA);
    TP_MODIFY_VALUE(eTP40_driver_right_err_sensor,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F6_KEYA);
    TP_MODIFY_VALUE(eTP41_driver_right_err_sensor,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F7_KEYA);
    TP_MODIFY_VALUE(eTP42_driver_right_err_load_config,err_KEYA[eM_R],err_KEYA[eM_R]|ERR_F8_KEYA);
#endif

    Bit(errBuf,MOTOR_DRIVER_ERR_SUPERHEAT_L,err_KEYA[eM_L]&ERR_F1_KEYA);							//过热
    Bit(errBuf,MOTOR_DRIVER_ERR_SUPERHEAT_R,err_KEYA[eM_R]&ERR_F1_KEYA);
    Bit(errBuf,MOTOR_DRIVER_ERR_HALL_L,err_KEYA[eM_L]&ERR_F6_KEYA);									//霍尔错误
    Bit(errBuf,MOTOR_DRIVER_ERR_HALL_R,err_KEYA[eM_R]&ERR_F6_KEYA);
    Bit(errBuf,MOTOR_DRIVER_ERR_MOS_L,err_KEYA[eM_L]&ERR_F7_KEYA);										//MOS损坏
    Bit(errBuf,MOTOR_DRIVER_ERR_MOS_R,err_KEYA[eM_R]&ERR_F7_KEYA);
    Bit(errBuf,MOTOR_DRIVER_ERR_UNDERVOLTAGE,err_KEYA[eM_L]|err_KEYA[eM_R]&ERR_F3_KEYA);//欠压
    Bit(errBuf,MOTOR_DRIVER_ERR_OVERVOLTAGE,err_KEYA[eM_L]|err_KEYA[eM_R]&ERR_F2_KEYA);//过压
    Bit(errBuf,MOTOR_STATE_ERR_SCRAM,err_KEYA[eM_L]|err_KEYA[eM_R]&ERR_F5_KEYA);//紧急停止
    Bit(errBuf,MOTOR_DRIVER_ERR_LOAD_CFG,err_KEYA[eM_L]|err_KEYA[eM_R]&ERR_F8_KEYA);//启动时加载的默认配置错误

    //短路告警增加时延检测 
    //Bit(errBuf,MOTOR_DRIVER_ERR_OVERCURRENT_L,err_KEYA[eM_L]&ERR_F4_KEYA);						//过流
    //Bit(errBuf,MOTOR_DRIVER_ERR_OVERCURRENT_R,err_KEYA[eM_R]&ERR_F4_KEYA);
    check_driver_short_circuit(
        &errBuf
        ,(err_KEYA[eM_L]&ERR_F4_KEYA)
        ,(err_KEYA[eM_R]&ERR_F4_KEYA));

    
    //	WriteBit

    static unsigned char err_KEYALast[2];
    if( (err_KEYA[eM_L]!=err_KEYALast[eM_L])
            ||((err_KEYA[eM_R]!=err_KEYALast[eM_R])) )
    {
        setErrCom_KEYA(eb16_lOverHeatErr,err_KEYA[eM_L]&ERR_F1_KEYA);
        setErrCom_KEYA(eb08_rOverHeatErr,err_KEYA[eM_R]&ERR_F1_KEYA);
        setErrCom_KEYA(eb17_lOverV_Err,err_KEYA[eM_L]&ERR_F2_KEYA);
        setErrCom_KEYA(eb09_rOverV_Err,err_KEYA[eM_R]&ERR_F2_KEYA);
        setErrCom_KEYA(eb18_lUnderV_Err,err_KEYA[eM_L]&ERR_F3_KEYA);
        setErrCom_KEYA(eb10_rUnderV_Err,err_KEYA[eM_R]&ERR_F3_KEYA);
        setErrCom_KEYA(eb20_lEmgyStopFatal,err_KEYA[eM_L]&ERR_F5_KEYA);
        setErrCom_KEYA(eb12_rEmgyStopFatal,err_KEYA[eM_R]&ERR_F5_KEYA);
        setErrCom_KEYA(eb21_lBrushSensorFatal,err_KEYA[eM_L]&ERR_F6_KEYA);
        setErrCom_KEYA(eb13_rBrushSensorFatal,err_KEYA[eM_R]&ERR_F6_KEYA);
        setErrCom_KEYA(eb22_lMosfetFatal,err_KEYA[eM_L]&ERR_F7_KEYA);
        setErrCom_KEYA(eb14_rMosfetFatal,err_KEYA[eM_R]&ERR_F7_KEYA);
        setErrCom_KEYA(eb23_lStartLoadConfFatal,err_KEYA[eM_L]&ERR_F8_KEYA);
        setErrCom_KEYA(eb15_rStartLoadConfFatal,err_KEYA[eM_R]&ERR_F8_KEYA);
        
       // setErrCom_KEYA(eb19_lShortCircuitFatal,err_KEYA[eM_L]&ERR_F4_KEYA);
       // setErrCom_KEYA(eb11_rShortCircuitFatal,err_KEYA[eM_R]&ERR_F4_KEYA);
    }
    err_KEYALast[eM_L] = err_KEYA[eM_L];
    err_KEYALast[eM_R] = err_KEYA[eM_R];
    /*end***************************/
    return errBuf;
    //返回Err状态
}

float readDistance_KEYA(unsigned char motor_addr)
{
    if(motor_addr==KEYA_ADDR_LEFT)
        return distance_KEYA[eM_L];
    else
        return distance_KEYA[eM_R];
}

float readElec_KEYA(unsigned char motor_addr)
{
    //发出CAN查询
    CAN_TxHeaderTypeDef TxMessage;
    TxMessage.DLC=4;
    TxMessage.StdId=0x601;
    TxMessage.IDE=CAN_ID_STD;//IDE位为标识符鿉择位（0，使用标准标识符＿1，使用扩展标识符＿
    TxMessage.RTR=CAN_RTR_DATA;//RTR位用于标识是否是远程
    static uint8_t TR_BUF[4]= {0x40,0x00,0x21,0x00};

    TR_BUF[3] = motor_addr;

    SendCAN_KEYA(&TxMessage,TR_BUF);

    if(motor_addr==KEYA_ADDR_LEFT)
        return elec_KEYA[0];
    else
        return elec_KEYA[1];
}

float readVolt_KEYA(void)
{
	  //发出CAN查询
    CAN_TxHeaderTypeDef TxMessage;
    TxMessage.DLC=4;
    TxMessage.StdId=0x601;
    TxMessage.IDE=CAN_ID_STD;//IDE位为标识符鿉择位（0，使用标准标识符＿1，使用扩展标识符＿
    TxMessage.RTR=CAN_RTR_DATA;//RTR位用于标识是否是远程
    static uint8_t TR_BUF[4]= {0x40,0x0D,0x21,0x00};

    TR_BUF[3] = 0x02;	//电池电压

    SendCAN_KEYA(&TxMessage,TR_BUF);
		
		return vlot_KEYA;
}

void writeSpeed_KEYA(float pwm,unsigned char motor_addr)
{
    CAN_TxHeaderTypeDef TxMessage;
    TxMessage.DLC=8;
    TxMessage.StdId=0x601;
    TxMessage.IDE=CAN_ID_STD;//IDE位为标识符鿉择位（0，使用标准标识符＿1，使用扩展标识符＿
    TxMessage.RTR=CAN_RTR_DATA;//RTR位用于标识是否是远程
    static uint8_t TR_BUF[8]= {0x23,0x00,0x20,0x02,0,0,0,0};

    TR_BUF[3] = motor_addr;

    long PWM = pwm*1000;

    TR_BUF[4] = (uint8_t)PWM;
    TR_BUF[5] = (uint8_t)(PWM>>8);



    SendCAN_KEYA(&TxMessage,TR_BUF);
}
/*************************************************
Function: 			readSpeed_KEYA
Description:		读取科亚电驱运行速度
Input:					motor_addr:左右电机
Output: 				none
Return: 				返回左右电机速度
*************************************************/
float readSpeed_KEYA(unsigned char motor_addr)
{
    static uint8_t TR_BUF[4]= {0x40,0x0A,0x21,0x01};
    CAN_TxHeaderTypeDef TxMessage;
    TxMessage.DLC=4;
    TxMessage.StdId=0x601;
    TxMessage.IDE=CAN_ID_STD;//IDE位为标识符鿉择位（0，使用标准标识符＿1，使用扩展标识符＿
    TxMessage.RTR=CAN_RTR_DATA;//RTR位用于标识是否是远程

    TR_BUF[3] = motor_addr;

    SendCAN_KEYA(&TxMessage,TR_BUF);

    if(motor_addr==KEYA_ADDR_LEFT)
        return speed_KEYA[eM_L];
    else
        return speed_KEYA[eM_R];
}
void canCallback_KEYA(uint8_t data[8])
{
    if (data[1] == 0x12 && data[2] == 0x21 )
    {
        errCallback_KEYA(data);
    }

    if (data[1] == 0x0A && data[2] == 0x21)
    {
        speedCallback_KEYA(data);
    }

    if(data[1] == 0x00 && data[2] == 0x21)
    {
        elecCallback(data);
    }
		if(data[1] == 0x0D && data[2] == 0x21)
		{
				voltCallback(data);
		}

}
void errCallback_KEYA(uint8_t data[8])
{
    switch(data[3])
    {
    case 0:
        err_KEYA[eM_L] = data[4];
        err_KEYA[eM_R] = data[4];
        break;
    }
}
void speedCallback_KEYA(uint8_t data[8])
{
    short int speed_buf;
    static float timebuf[2];
    float Ts;

    timebuf[0] = timebuf[1];
    timebuf[1] = getSystemTick()/1000.0f;

    Ts = timebuf[1] - timebuf[0];

    distance_KEYA[eM_L] += Ts*speed_KEYA[eM_L];
    distance_KEYA[eM_R] += Ts*speed_KEYA[eM_R];

    switch(data[3])
    {
    case KEYA_ADDR_LEFT:
        speed_buf = (data[5]<<8) + data[4];
        speed_KEYA_Filter[eM_L].inData = speed_buf/60.0f/numPloe_KEYA*0.95f;
        speed_KEYA[eM_L] = extremeFilter(&speed_KEYA_Filter[eM_L]);
        break;
    case KEYA_ADDR_RIGHT:
        speed_buf = (data[5]<<8) + data[4];
        speed_KEYA_Filter[eM_R].inData =  -speed_buf/60.0f/numPloe_KEYA*0.95f;
        speed_KEYA[eM_R] =extremeFilter(&speed_KEYA_Filter[eM_R]);
        break;
    }

}
void elecCallback(uint8_t data[8])
{
    short int buf;
    buf = (data[5]<<8) + data[4];
    switch(data[3])
    {
    case KEYA_ADDR_LEFT:
        elec_KEYA[eM_L] = -buf/10.0f;
        break;
    case KEYA_ADDR_RIGHT:
        elec_KEYA[eM_R] = buf/10.0f;
        break;
    }
}
void voltCallback(uint8_t data[8])
{
		short int buf;
    buf = (data[5]<<8) + data[4];
		vlot_KEYA = buf/10.0f + (48 - 36.7);
}
void keya_stop(void)
{
  writeSpeed_KEYA(0,KEYA_ADDR_LEFT);
  writeSpeed_KEYA(0,KEYA_ADDR_RIGHT);
}

void check_driver_short_circuit(uint16_t *buf,uint8_t err_L,uint8_t err_R)
{
    static uint8_t errF4_L_cnt=0 ,errF4_R_cnt = 0;
    if(err_L)
    {
      if(errF4_L_cnt < KEYA_LOG_MAX_CNT)
      {
        errF4_L_cnt++;
      }
      else if(errF4_L_cnt == KEYA_LOG_MAX_CNT)
      {
        Bit(*buf,MOTOR_DRIVER_ERR_OVERCURRENT_L,ALARM_ERR);
        setAlarm(eA_DRIVER,eb19_lShortCircuitFatal,ALARM_ERR);
        errF4_L_cnt = KEYA_LOG_OUT;
        GR_LOG_ERROR("Left driver ShortCircuit");
      }
    }
    else
    {
      if(errF4_L_cnt == KEYA_LOG_OUT)
      {
         GR_LOG_INFO("Left driver ShortCircuit recover");
      }
      errF4_L_cnt = 0;
      Bit(*buf,MOTOR_DRIVER_ERR_OVERCURRENT_L,ALARM_OK);
      setAlarm(eA_DRIVER,eb19_lShortCircuitFatal,ALARM_OK);
    }

    if(err_R)
    {
      if(errF4_R_cnt < KEYA_LOG_MAX_CNT)
      {
        errF4_R_cnt++;
      }
      else if(errF4_R_cnt == KEYA_LOG_MAX_CNT)
      {
        Bit(*buf,MOTOR_DRIVER_ERR_OVERCURRENT_R,ALARM_ERR);
        setAlarm(eA_DRIVER,eb11_rShortCircuitFatal,ALARM_ERR);
        errF4_R_cnt = KEYA_LOG_OUT;
        GR_LOG_ERROR("Right driver ShortCircuit");
      }
    }
    else
    {
      if(errF4_R_cnt == KEYA_LOG_OUT)
      {
         GR_LOG_INFO("Right driver ShortCircuit recover");
      }
      errF4_R_cnt = 0;
      Bit(*buf,MOTOR_DRIVER_ERR_OVERCURRENT_R,ALARM_OK);
      setAlarm(eA_DRIVER,eb11_rShortCircuitFatal,ALARM_OK);
    }
}


