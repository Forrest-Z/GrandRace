#include "Ultrasonic_New.h"
#include <string.h>
#include "Subscribe_Pc.h"
#include "Clock.h"
#include "OD.h"
#include "SD_Log_App.h"
#include "log_app.h"
#include <stdlib.h>
#include "chassis_com.h"

static stUltrasonicData ultrasonic_data = {0, {0}};

void receiveUltrasonicSampleData(uint8_t group_cnt, uint8_t *pmsg)
{
    if((ultrasonic_data.count >= ULTRASONIC_MAX_CNT)
            || ((ultrasonic_data.count + group_cnt) >= ULTRASONIC_MAX_CNT))
    {
        return;
    }

    memcpy(&ultrasonic_data.databuf[ultrasonic_data.count * 3], pmsg, (group_cnt * 3));
    ultrasonic_data.count += group_cnt;

}

unsigned char dataCheckSum(uint8_t *pData, uint8_t len)
{
    unsigned char checksum = 0;
    unsigned char i;

    for(i = 0; i < len; i++)
    {
        checksum += *pData++;
    }

    return checksum;
}

extern void UltrasonicPubTestHandle(uint8_t *pData, uint8_t cnt);

void pubUltrasonicSampleData(void)
{
    static unsigned long int t = 0;
    uint8_t cnt, i, group_cnt, last_cnt, data_num;
    uint8_t pub_buffer[64] = {0};

    cnt = ultrasonic_data.count;
    if(cnt == 0)
        return;

//	if(cnt > 60)
//		cnt = 60;

    last_cnt = (cnt % 12);
    group_cnt = (cnt / 12) + (last_cnt > 0);

    pub_buffer[0] = 0x3A;
    pub_buffer[1] = 0x0C;
    pub_buffer[2] = 0x01;

    for(i = 0; i < (group_cnt - 1); i++)
    {
        pub_buffer[3] = (12 * 3) + 1;
        pub_buffer[4] = 12;
        memcpy(&pub_buffer[5], &ultrasonic_data.databuf[i * 36], 36);
        pub_buffer[41] = dataCheckSum(&pub_buffer[1], 40);
        pub_buffer[42] = 0x7E;

        getSystemTimeStamp(t++, &Ultrasonic_Up_Pub_Buffer.time);
        Ultrasonic_Up_Pub_Buffer.len = 43;
        memcpy(Ultrasonic_Up_Pub_Buffer.info, pub_buffer, 43);
	      publishFrameData(TX2_ID, ULTRASONIC_UP_ID, (unsigned char *)&Ultrasonic_Up_Pub_Buffer, 43 + 2 + 12);
//      UltrasonicPubTestHandle(&ultrasonic_data.databuf[i * 36], 12);
    }

    data_num = (cnt - i*12) * 3;
    pub_buffer[3] = data_num + 1;
    pub_buffer[4] = (cnt - i*12);
    memcpy(&pub_buffer[5], &ultrasonic_data.databuf[i * 36], data_num);
    pub_buffer[5 + data_num] = dataCheckSum(&pub_buffer[1], data_num + 4);
    pub_buffer[6 + data_num] = 0x7E;

    getSystemTimeStamp(t++, &Ultrasonic_Up_Pub_Buffer.time);
    Ultrasonic_Up_Pub_Buffer.len = 6 + pub_buffer[3];
    memcpy(Ultrasonic_Up_Pub_Buffer.info, pub_buffer, Ultrasonic_Up_Pub_Buffer.len);
    publishFrameData(TX2_ID, ULTRASONIC_UP_ID, (unsigned char *)&Ultrasonic_Up_Pub_Buffer, Ultrasonic_Up_Pub_Buffer.len + 2 + 12);
//  UltrasonicPubTestHandle(&ultrasonic_data.databuf[i * 36], pub_buffer[4]);

    ultrasonic_data.count -= cnt;
}


uint16_t getUltrasonicState(void)
{
    uint16_t utl_state = 0;
		//uint8_t i = 0;
    if(getUltrasonicCommunicationState() == 0)
    {
        return (0x01 << 0);	//(0x01 << 12);
    }

    //0位代表通信 1开始代码通道
    utl_state = getUltrasonicModuleState();
    utl_state <<= 1;
		
    //检测使能位
//    for ( i =0;i < 12;i++)
//    {
//      if(g_EnvCfg.sENV.ult[i] == OFF)
//        WriteBit(utl_state,(i+1),0);
//    }
    
    return utl_state;
}

// 采用交互命令的方式获取超声波采集板各个探头通道的工作状态
void getUltrasonicChannelState(void)
{
    const unsigned char check_cmd_buff[6] = {0x3A, 0x0D, 0x00, 0x00, 0x0D, 0x7E};
//    static unsigned char flag = 0;

    if(getUltrasonicCommunicationState() == 0)
    {
//        flag = 0;
        return;
    }

//    if(flag == 0)
//    {
//        flag = 1;
        Ultrasonic_Sample_Send_Buffer.len = 6;
        memcpy(Ultrasonic_Sample_Send_Buffer.info, check_cmd_buff, 6);
        publishFrameData(ULT_ID, PUB_ULT_SAMPLE_SEND_ID, (unsigned char *)&Ultrasonic_Sample_Send_Buffer, 6 + 2);
//    } 
}

void ult_VersionCallback(void)
{
	  if (!g_bVersion)
      sdlog_printf_app("%s" ,ult_version);
		GR_LOG_INFO("%s" ,ult_version);
}
void ult_ChannelStatusCallback(void)
{
    sdlog_printf_app("callbak ult ch=%d en=%d",g_Ult_status.ch,g_Ult_status.en);
		GR_LOG_INFO("callbak ult ch=%d en=%d",g_Ult_status.ch,g_Ult_status.en);
}

#if 0
void getUltrasonicRunState(void)
{
    const unsigned char check_cmd_buff[6] = {0x3A, 0x0E, 0x00, 0x00, 0x0E, 0x7E};

    if(getUltrasonicCommunicationState() == 0)
    {
        return;
    }

    Ultrasonic_Sample_Send_Buffer.len = 6;
    memcpy(Ultrasonic_Sample_Send_Buffer.info, check_cmd_buff, 6);
    publishFrameData(ULT_ID, PUB_ULT_SAMPLE_SEND_ID, (unsigned char *)&Ultrasonic_Sample_Send_Buffer, 6 + 2);
}
#endif
