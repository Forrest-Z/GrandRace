#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "OD.h"
#include "info_OD.h"
#include "info_core.h"
#include "info_crc.h"
#include "stm32f4xx_hal.h"
#include "Clock.h"
#include "Publish_Pc.h"
#include "chassis_config.h"
#include "Frame_statistical.h"
#include "chassis_fsm.h"
#include "SD_Log_App.h"
#include "chassis_com.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif


#ifndef __COMMUN_USER_NET
uint8_t Reco_flag = 0;
#endif
COM_STRUCK sCom;
COMLOGFLAG comlogflag = {0,0,0,0};
static eCOMM_STATE m_comm_state;

void initTransferProtocol(unsigned char device_type,unsigned char device_id)
{
    int publish_num = 0;
    int size_s,size_p,size_h;

    size_p = cal_Publish_size();
    size_s = cal_subscribe_size();
    size_h = cal_heartSingal_size();

    sCom.device_type = device_type;
    sCom.device_id = device_id;


    for(int i = 0; i<size_h; i++)
    {
        sCom.pHeart[i] = (HEART_SINGAL_STRUCT*)(heart_singal_struct+i);
    }

    for(publish_num = 0; publish_num<size_h; publish_num++)
    {
        sCom.pPub[publish_num] = (Publish_struct*)(heartSingal+publish_num);
        if(sCom.pPub[publish_num]->initCallBack != NULL)
        {
            sCom.pPub[publish_num]->initCallBack();
        }
    }
    for(int i = 0; i<size_s; i++)
    {
        sCom.pSub[i] = (Subscribe_struct*)(subscribe+i);
        if(sCom.pSub[i]->initCallBack != NULL)
        {
            sCom.pSub[i]->initCallBack();
        }
    }
    for(int i = 0; i<size_p; i++)
    {
        sCom.pPub[publish_num] = (Publish_struct*)(publish+i);
        if(sCom.pPub[publish_num]->initCallBack != NULL)
        {
            sCom.pPub[publish_num]->initCallBack();
        }
        publish_num++;
    }
    //printf("init finsh\n");
}




unsigned long int getSysTime(void)
{
    unsigned long int time;
    time = readSysTime_ms();
    return time;
}

unsigned char convertHeartSingalIdTonum(unsigned char id_m,unsigned char id_s)
{
    unsigned char num,i;

    num = cal_heartSingal_size();
    for(i=0; i<num; i++)
    {
        if(sCom.pHeart[i]->id_m == id_m)  // && sCom.pHeart[i]->id_s == id_s
        {
            break;
        }
    }
    if(i<num)
    {
        return i;
    }
    else
    {
        return 255;
    }
}

unsigned char convertPublishIdTonum(unsigned char id_m,unsigned char id_s)
{
    unsigned char num,i;

    num = cal_Publish_size() + cal_heartSingal_size();

    for(i=0; i<num; i++)
    {
        if(sCom.pPub[i]->id_m == id_m && sCom.pPub[i]->id_s == id_s)
        {
            break;
        }
    }
    if(i<num)
    {
        return i;
    }
    else
    {
        return 255;
    }
}

unsigned char convertSubscribeIdTonum(unsigned char id_m,unsigned char id_s)
{
    unsigned char num,i;
    num = cal_subscribe_size();
    for(i=0; i<num; i++)
    {
        if(sCom.pSub[i]->id_m == id_m && sCom.pSub[i]->id_s == id_s)
        {
            break;
        }
    }
    if(i<num)
    {
        return i;
    }
    else
    {
        return 255;
    }
}

void writeFrameHanlder(unsigned char id_m,unsigned char id_s,int length,unsigned char *data)
{
    unsigned char heart_singal_num;
    heart_singal_num = convertHeartSingalIdTonum(id_m,id_s);
    if(data != NULL && sCom.pHeart[heart_singal_num]->publishCallBack != NULL)
    {
        sCom.pHeart[heart_singal_num]->publishCallBack(length,data);
    }
}

void resetACKStatus(unsigned char id_m,unsigned char id_s)
{
    unsigned char pubNum = convertPublishIdTonum(id_m,id_s);
    if(pubNum != 255)
    {
        sCom.pPub[pubNum]->sFrame->sAck.send_ack = 0;
        sCom.pPub[pubNum]->sFrame->sAck.resend_num = 0;
    }

}

int publishHanlder(FRAME_DATA *send_frame);
unsigned long int overtime_cnt = 0;

//等待ACK应答信号，如果发送超时将会把上一次的数据重新发出。
int waitAckData(unsigned char id_m,unsigned char id_s)
{
    unsigned char i;
    //i = id;
    i = convertPublishIdTonum(id_m,id_s);  //将ID转变为序列号
    if(i == 255)
    {
        return -4;
    }
    if(sCom.pPub[i]->sFrame->sAck.send_ack == 1)
    {
        sCom.pPub[i]->sFrame->sAck.curTime = getSysTime();
        if(sCom.pPub[i]->sFrame->sAck.curTime
                - sCom.pPub[i]->sFrame->sAck.send_time
                > sCom.pPub[i]->sFrame->sAck.over_time)
        {
            //表示未收到校验信息，发送失败。
            int send_max_num = 0;
            if(id_s <= PUBLISH_MAX_SYS_ID)
            {
                send_max_num = sCom.pPub[i]->sFrame->sAck.resend_set_num + 1;
            }
            else
            {
                send_max_num = sCom.pPub[i]->sFrame->sAck.resend_set_num;
            }
            if(sCom.pPub[i]->sFrame->sAck.resend_num < send_max_num)
            {

                sCom.pPub[i]->sFrame->sAck.send_time =
                    sCom.pPub[i]->sFrame->sAck.curTime;
                sCom.pPub[i]->sFrame->sAck.send_ack = 0;
                //再次发送数据
                publishHanlder(&sCom.pPub[i]->sFrame->sFrameData);
                if(sCom.pPub[i]->sFrame->sAck.resend_num <
                        sCom.pPub[i]->sFrame->sAck.resend_set_num)
                {
                    sCom.pPub[i]->sFrame->sAck.resend_num++;
                }
                overtime_cnt++;

                return -1;
            }
            else
            {
                return -2;
            }
        }
    }
    return 0;
}

unsigned char Cal_transmitId(FRAME_DATA *send_frame)
{
    unsigned char i;
    i = send_frame->frame_id_m;
    return i;
}

unsigned char send_frame_data[FRAME_SIZE];

int publishACKData(FRAME_DATA *send_frame)
{
    int data_length;
    if(send_frame == NULL)
    {
        return -3;
    }
    //unsigned char send_frame_data[FRAME_SIZE];

    unsigned char des_id = send_frame->frame_id_m;

    send_frame->frame_id_m = sCom.device_id;


    send_frame_data[0] = HEAD_DATA1;
    send_frame_data[1] = HEAD_DATA2;

    unsigned char src_data[FRAME_SIZE];
    memcpy(src_data,&send_frame->frame_size,send_frame->frame_size + 6);
    Cal_CRC(send_frame->crc,src_data,send_frame->frame_size + 6);

    memcpy(&send_frame_data[2], send_frame, FRAME_HEAD_SIZE-2);

    for(int i = 0; i < send_frame->frame_size; i++)
    {
        send_frame_data[i +FRAME_HEAD_SIZE] = send_frame->frame_buffer[i];
    }

    send_frame_data[send_frame->frame_size + FRAME_HEAD_SIZE]     = TAIL_DATA1;
    send_frame_data[send_frame->frame_size + FRAME_HEAD_SIZE + 1] = TAIL_DATA2;

    data_length = send_frame->frame_size + FRAME_HEAD_SIZE + 2;
    unsigned char transmit_id;
    transmit_id = des_id;//Cal_transmitId(send_frame);


    writeFrameHanlder(transmit_id,0,data_length,send_frame_data);
    return 0;
}

//发送处理函数，用于生成一个发送帧并将数据发出
int publishHanlder(FRAME_DATA *send_frame)
{
    int data_length;
    if(send_frame == NULL)
    {
        return -3;
    }



    unsigned char pubNum = convertPublishIdTonum(send_frame->frame_id_m,send_frame->frame_id_s);
    if(pubNum == 255)
    {
        return -1;
    }



    if(send_frame->ack == 1)  //该帧数据为需要校验的数据结构
    {
        if(sCom.pPub[pubNum]->sFrame->sAck.send_ack == 1)
        {
            //表示上条信息发出后未收到应答信号,并将上条消息重新发出
            return 0;//waitAckData(send_frame->frame_id);
        }
        else
        {
            sCom.pPub[pubNum]->sFrame->sAck.send_ack = 1;  //将该帧数据置为未校验
            sCom.pPub[pubNum]->sFrame->sAck.curTime = getSysTime();
            sCom.pPub[pubNum]->sFrame->sAck.send_time
                = sCom.pPub[pubNum]->sFrame->sAck.curTime;
        }

        //将需要发送的数据存储到发送结构体中
        unsigned char ack;
        ack = sCom.pPub[pubNum]->sFrame->sFrameData.ack;
        memcpy(&sCom.pPub[pubNum]->sFrame->sFrameData,send_frame,sizeof(FRAME_DATA));
        sCom.pPub[pubNum]->sFrame->sFrameData.ack = ack;
    }

    unsigned char des_id = send_frame->frame_id_m;  //des

    send_frame->frame_id_m = sCom.device_id;  //src
    //unsigned char send_frame_data[FRAME_SIZE];

    send_frame_data[0] = HEAD_DATA1;
    send_frame_data[1] = HEAD_DATA2;

    unsigned char src_data[FRAME_SIZE];
    if(send_frame->frame_size + 6 >= FRAME_SIZE)
    {
        //数据大小计算错误
        return -2;
    }
    memcpy(src_data,&send_frame->frame_size,send_frame->frame_size + 6);
    Cal_CRC(send_frame->crc,src_data,send_frame->frame_size + 6);

    memcpy(&send_frame_data[2], send_frame, FRAME_HEAD_SIZE-2);

    for(int i = 0; i < send_frame->frame_size; i++)
    {

        send_frame_data[i +FRAME_HEAD_SIZE] = send_frame->frame_buffer[i];
    }

    send_frame_data[send_frame->frame_size + FRAME_HEAD_SIZE]     = TAIL_DATA1;
    send_frame_data[send_frame->frame_size + FRAME_HEAD_SIZE + 1] = TAIL_DATA2;

    data_length = send_frame->frame_size + FRAME_HEAD_SIZE + 2;
    unsigned char transmit_id;
    transmit_id = des_id;//Cal_transmitId(send_frame);
    send_frame->frame_id_m = des_id;
//	unsigned char id_num;
//	id_num = convertHeartSingalIdTonum(transmit_id,0);
//	if(send_frame->frame_id_s!= 50)
//	{
    if((send_frame->frame_id_s > PUBLISH_MAX_SYS_ID) && (sCom.comState[transmit_id] == 0))
    {
        //通讯断开数据停止发送
        return -1;
    }
//  }
    writeFrameHanlder(transmit_id,0,data_length,send_frame_data);
    //memset(send_frame_data, 0, sizeof(send_frame_data));

    return 0;
}


//发送一帧数据
void publishFrameData(unsigned char id_m,unsigned char id_s, unsigned char *data, int size)
{
    FRAME_DATA send_frame;
    unsigned char pubNum = convertPublishIdTonum(id_m,id_s);
    if(pubNum == 255 || data == NULL)
    {
        return;
    }
    send_frame.frame_id_m=id_m;
    send_frame.frame_id_s=id_s;
    send_frame.frame_size = size;
    send_frame.reserved[0] = 0xff;
    send_frame.reserved[1] = 0xff;
    send_frame.ack = sCom.pPub[pubNum]->sFrame->sFrameData.ack;

    memcpy(send_frame.frame_buffer,data,send_frame.frame_size);
    publishHanlder(&send_frame);
}

unsigned long int  test_cnt = 0;
float err_bate = 0.0;
unsigned long int err_cnt = 0;
#ifndef __COMMUN_USER_NET
unsigned char first_run_flag = 0;
#endif
//处理接收到的一帧数据
uint8_t recon_time = 0;
uint8_t snd_timeoutlog = 0;
int frameDealHanlder(unsigned char *data)
{
    static uint8_t rcv_timeoutlog = 0;
    static uint8_t rcv_begin = 0;
    static uint32_t rev_heart_current_time = 0;
    static uint32_t rev_heart_last_time = 0;
    static uint32_t rev_heart_diff_time = 0;

    FRAME_DATA frame_data;

    frame_data.frame_size = data[4];
    frame_data.frame_id_m = data[7];
    frame_data.frame_id_s = data[8];
    frame_data.ack =data[9];

    unsigned char subNum = convertSubscribeIdTonum(frame_data.frame_id_m,frame_data.frame_id_s);
    unsigned char pubNum = convertPublishIdTonum(frame_data.frame_id_m,frame_data.frame_id_s);

    rev_heart_current_time = getSysTime();
    rev_heart_diff_time = rev_heart_current_time - rev_heart_last_time;

    if((rev_heart_diff_time>2700)&&(rcv_begin))
    {
        if(!rcv_timeoutlog)
        {
            GR_LOG_ERROR("Rcv_heartsingle timeout,rev_heartdifftime = %d",rev_heart_diff_time);
            rcv_timeoutlog = 1;
            //连接超时
            m_comm_state.flag.b01_timeout_err = COM_STATE_ERR;
        }
    }

    if((pubNum == 255 && subNum == 255)   //&& frame_data.frame_id_s != 1
            || (frame_data.ack != 0 && frame_data.ack != 1))
    {
        return 0;
    }

    for(int i=FRAME_HEAD_SIZE; i<frame_data.frame_size + FRAME_HEAD_SIZE; i++)
    {
        frame_data.frame_buffer[i-FRAME_HEAD_SIZE] = data[i];
    }

    if(frame_data.ack == 1)   //应答
    {
        FRAME_DATA send_frame;
        send_frame.ack = 0;

        send_frame.frame_id_m = frame_data.frame_id_m;
        send_frame.frame_id_s = frame_data.frame_id_s;
        send_frame.frame_size = 4;

        send_frame.frame_buffer[0] = ACK_DATA1;
        send_frame.frame_buffer[1] = ACK_DATA2;
        send_frame.frame_buffer[2] = ACK_DATA3;
        send_frame.frame_buffer[3] = ACK_DATA4;
        publishACKData(&send_frame);     //发送ACK应答信号
    }
    if(pubNum !=255 && frame_data.ack == 0 && frame_data.frame_size == 4
            && frame_data.frame_buffer[0] == ACK_DATA1 && frame_data.frame_buffer[1] == ACK_DATA2
            && frame_data.frame_buffer[2] == ACK_DATA3 && frame_data.frame_buffer[3] == ACK_DATA4)  //表示该帧为ack应答数据
    {
        if(sCom.device_type == MASTER_DEVICE)
        {
            if(frame_data.frame_id_s == 0)  //表示该应答为心跳帧的应答
            {
                for(int i=0; i<cal_Publish_size() + cal_heartSingal_size(); i++) //通讯建立后将应答重置
                {
                    if(sCom.pPub[i]->id_s > PUBLISH_MAX_SYS_ID
                            && sCom.pPub[i]->id_m == frame_data.frame_id_m)
                    {
                        resetACKStatus(sCom.pPub[i]->id_m,sCom.pPub[i]->id_s);
                    }
                }
#ifndef __COMMUN_USER_NET
                if( (Reco_flag  == 1)&&(data[7] == 1))
                {
                    memset(&statistics_contotx2_msg,0,sizeof(STATISTICS_CONTOTX2_MSG));//发生重连，清空发送和接收帧统计
                    memset(&statistics_tx2tocon_msg,0,sizeof(STATISTICS_TX2toCON_msg));

                    statistics_tx2tocon_msg.sendTimesignal = 1;//因为时间同步帧先发了一帧
                    statistics_tx2tocon_msg.frame_total = 1;
                    Reco_flag = 0;
                    recon_time++;
                }
#endif
                if(data[7] == 1)//id_m
                {
                    statistics_tx2tocon_msg.heart_signal++;
                    statistics_tx2tocon_msg.frame_total++;
#ifndef __COMMUN_USER_NET
                    if(first_run_flag == 0)
                    {
                        statistics_contotx2_msg.heart_singnal++;
                        statistics_contotx2_msg.frame_total++;
                        first_run_flag = 1;
                    }
#endif
                }

                if(sCom.pPub[pubNum]->id_m == TX2_ID)
                {
                    comlogflag.TX2_COMLOGFLAG = 1;
                    rcv_begin = 1;
                    rcv_timeoutlog = 0;
                    snd_timeoutlog = 0;

                    //连接超时恢复
                    m_comm_state.flag.b01_timeout_err = COM_STATE_OK;

                    rev_heart_last_time = rev_heart_current_time;
                }
                else if(sCom.pPub[pubNum]->id_m == POW_ID)
                {
                    comlogflag.POWER_COMLOGFLAG = 1;
                }
                else if(sCom.pPub[pubNum]->id_m == JOYS_ID)
                {
                    comlogflag.JOY_COMLOGFLAG = 1;
                }
                else if(sCom.pPub[pubNum]->id_m == ULT_ID)
                {
                    comlogflag.ULT_COMLOGFLAG = 1;
                }

                sCom.comState[sCom.pPub[pubNum]->id_m] = 1;

                if(data[7] == 1)//id_m //重连成功后，上传上一次保存的帧数量，并Free缓存
                {
//					#ifdef __COMMUN_USER_NET
//						sendKeepalive(SOCK_TCPS);//add by huxi 发送keep_alive包,用于解决拔掉网线无法检测到的BUG
//					#endif
                    if(contotx2_buf != NULL)
                    {
                        publishFrameData(TX2_ID,CON_UP_SENDTOALE_ID, (uint8_t*)contotx2_buf, sizeof(STATISTICS_CONTOTX2_MSG));
                        Free_contotx2_buffer();
                        statistics_contotx2_msg.statistics_snd_42++;
                        statistics_contotx2_msg.frame_total++;
                    }
                    if(tx2tocon_buf != NULL)
                    {
                        publishFrameData(TX2_ID,CON_UP_RECIVTOALE_ID, (uint8_t*)tx2tocon_buf, sizeof(STATISTICS_TX2toCON_msg));
                        Free_tx2tocon_buffer();
                        statistics_contotx2_msg.statistics_rcv_43++;
                        statistics_contotx2_msg.frame_total++;
                    }
                }
            }
        }
        //设置发送帧的应答信号标志为已经收到状态
        sCom.pPub[pubNum]->sFrame->sAck.send_ack = 0;
        sCom.pPub[pubNum]->sFrame->sAck.resend_num = 0;
        test_cnt++;
        err_bate = (float)overtime_cnt / (float)test_cnt;

    }
    else if(pubNum !=255 && frame_data.ack == 1 && frame_data.frame_size == 4
            && frame_data.frame_buffer[0] == HEARTSINGAL_DATA1 && frame_data.frame_buffer[1] == HEARTSINGAL_DATA2
            && frame_data.frame_buffer[2] == HEARTSINGAL_DATA3 && frame_data.frame_buffer[3] == HEARTSINGAL_DATA4)  //表示该帧为心跳数据
    {
        //通讯正常

        if(sCom.device_type == SLAVE_DEVICE)
        {
            for(int i=0; i<cal_Publish_size() + cal_heartSingal_size(); i++) //通讯建立后将应答重置
            {
                if(sCom.pPub[i]->id_s > PUBLISH_MAX_SYS_ID
                        && sCom.pPub[i]->id_m == frame_data.frame_id_m)
                {
                    resetACKStatus(sCom.pPub[i]->id_m,sCom.pPub[i]->id_s);
                }
            }

            sCom.comState[sCom.pPub[pubNum]->id_m] = 1;

            sCom.pHeart[pubNum]->recordTime->lastTime = getSysTime();  //当当前设备为从机时会运行到这里
            sCom.pPub[pubNum]->sFrame->sAck.resend_num = 0;
        }
    }
    else if(pubNum !=255 && frame_data.ack == 0 && frame_data.frame_size == 14
            && frame_data.frame_id_m == 1 && frame_data.frame_id_s == 1)  //时钟同步
    {
        writeSystemTime(&frame_data.frame_buffer[4]);

        statistics_tx2tocon_msg.sendTimesignal++;
        statistics_tx2tocon_msg.frame_total++;

    }

    else if(subNum != 255)//用户部分的数据
    {
        memcpy(sCom.pSub[subNum]->data, frame_data.frame_buffer,frame_data.frame_size);
        if(sCom.pSub[subNum]->callback != NULL)
        {
            sCom.pSub[subNum]->callback();   //回调函数
        }
    }
    return 0;
}

//处理接收缓冲区中的数据
unsigned char receive_frame_data[FRAME_SIZE];
int receiveHanlder(BUFFER_DATA *buffer_data)
{

    int head,tail;
    if(buffer_data->RX_head - buffer_data->RX_tail>=0)
    {
        head = buffer_data->RX_head;
        tail = buffer_data->RX_tail;
    }
    else
    {
        head = buffer_data->RX_head + UART_BUFSIZE;
        tail = buffer_data->RX_tail;
    }
    if(head - tail<FRAME_HEAD_SIZE)
    {
        return -1;
    }

    for(int i = tail; i<= head; i++)
    {
        if(buffer_data->RX[buffer_data->RX_tail%UART_BUFSIZE] == HEAD_DATA1
                && buffer_data->RX[(buffer_data->RX_tail + 1)%UART_BUFSIZE] == HEAD_DATA2)
        {
            int rx_tail,rx_head;

            if(buffer_data->RX_head - buffer_data->RX_tail>=0)
            {
                rx_head = buffer_data->RX_head;
                rx_tail = buffer_data->RX_tail;
            }
            else
            {
                rx_head = buffer_data->RX_head + UART_BUFSIZE;
                rx_tail = buffer_data->RX_tail;
            }

            if(rx_head - rx_tail > FRAME_HEAD_SIZE - 4
                    && buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] < FRAME_DATA_SIZE)
            {
                unsigned char crc_data[2];
                unsigned char src_data[FRAME_SIZE];
                if(rx_head - rx_tail >= buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] + FRAME_HEAD_SIZE + 2)  //接收到一帧数据
                {
                    if(buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] + FRAME_HEAD_SIZE + 2 < FRAME_SIZE)
                    {
                        for(int j=0; j<buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] + FRAME_HEAD_SIZE + 2; j++)
                        {
                            receive_frame_data[j] = buffer_data->RX[(buffer_data->RX_tail + j)%UART_BUFSIZE];
                        }
                    }
                    else
                    {
                        //数据传输错误
                        err_cnt++;
                        return -2;
                    }
                    if(receive_frame_data[4] + 6 < FRAME_SIZE)
                    {
                        memcpy(src_data,&receive_frame_data[4],receive_frame_data[4] + 6);
                        Cal_CRC(crc_data,src_data,receive_frame_data[4] + 6);
                    }
                    else
                    {
                        //数据传输错误
                        err_cnt++;
                        return -2;
                    }


                    int frame_size = receive_frame_data[4] + FRAME_HEAD_SIZE + 2;

                    if(frame_size>FRAME_SIZE)
                    {
                        //数据传输错误
                        err_cnt++;
                        return -2;
                    }
                    if(crc_data[0] == receive_frame_data[2] && crc_data[1] == receive_frame_data[3]
                            &&receive_frame_data[frame_size - 2]  == TAIL_DATA1
                            && receive_frame_data[frame_size - 1] == TAIL_DATA2)  //CRC校验
                    {
                        i = i + buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] + FRAME_HEAD_SIZE - 1 + 2;
                        buffer_data->RX_tail = (buffer_data->RX_tail + buffer_data->RX[(buffer_data->RX_tail + 4)%UART_BUFSIZE] + FRAME_HEAD_SIZE ) % UART_BUFSIZE;
                        frameDealHanlder(receive_frame_data);  //处理该帧中的数据
                        if(head - i<FRAME_HEAD_SIZE)
                        {
                            break;
                        }
                    }
                    else  //CRC校验失败
                    {
                        buffer_data->RX_tail = (buffer_data->RX_tail + 1)  % UART_BUFSIZE;
                    }
                }
                else  //还未完整接收一帧数据
                {
                    return 0;
                }
            }
            else if(rx_head - rx_tail > FRAME_HEAD_SIZE - 4)   //数据格式不对
            {
                buffer_data->RX_tail = (buffer_data->RX_tail + 1)  % UART_BUFSIZE;
            }
            else
            {
                return 0;  //还未完整接收一帧数据
            }
        }
        else if(head - i>FRAME_HEAD_SIZE) //还没有找到帧头
        {
            buffer_data->RX_tail = (buffer_data->RX_tail + 1)  % UART_BUFSIZE;
        }
    }
    return 0;
}

void PublishHeartSingal(void)
{
    FRAME_DATA send_frame;
    static uint32_t heart_current_time =0;
    static uint32_t heart_last_time =0;
    static uint32_t heart_time_diff = 0;

    heart_current_time = getSysTime();
    heart_time_diff = heart_current_time - heart_last_time;
    if((heart_time_diff > 6000)&&( !snd_timeoutlog ))
    {
        GR_LOG_ERROR("Snd_heartsingal timeout,snd_heartdifftime = %d",heart_time_diff);
        snd_timeoutlog = 1;
    }

    for(int i = 0; i<cal_heartSingal_size(); i++)
    {
        if(sCom.pHeart[i]->id_s == 0)
        {
            unsigned char pubNum = convertPublishIdTonum(sCom.pHeart[i]->id_m,sCom.pHeart[i]->id_s);
            send_frame.frame_id_m = sCom.pHeart[i]->id_m;
            send_frame.frame_id_s = sCom.pHeart[i]->id_s;
            send_frame.frame_size = 4;
            send_frame.reserved[0] = 0xff;
            send_frame.reserved[1] = 0xff;
            send_frame.ack = sCom.pPub[pubNum]->sFrame->sFrameData.ack;
            send_frame.frame_buffer[0] = HEARTSINGAL_DATA1;
            send_frame.frame_buffer[1] = HEARTSINGAL_DATA2;
            send_frame.frame_buffer[2] = HEARTSINGAL_DATA3;
            send_frame.frame_buffer[3] = HEARTSINGAL_DATA4;
            sCom.pHeart[i]->recordTime->curTime = getSysTime();
            if(sCom.pHeart[i]->recordTime->curTime
                    - sCom.pHeart[i]->recordTime->lastTime
                    > sCom.pHeart[i]->recordTime->offsetTime)
            {
                publishHanlder(&send_frame);
                sCom.pHeart[i]->recordTime->lastTime = getSysTime();
                heart_last_time = heart_current_time;
            }
        }
    }
}

void setconnectedState(void)
{
    for(int i = 0; i<cal_heartSingal_size(); i++)
    {
        if(sCom.device_type == SLAVE_DEVICE)
        {
            sCom.pHeart[i]->recordTime->curTime = getSysTime();
            if(sCom.pPub[i]->sFrame->sAck.resend_num == 0)
            {
                if(sCom.pHeart[i]->recordTime->curTime
                        - sCom.pHeart[i]->recordTime->lastTime
                        > sCom.pHeart[i]->recordTime->offsetTime + 1000/TX2_HEARTSINGAL_HZ)
                {
                    sCom.pHeart[i]->recordTime->lastTime = getSysTime();
                    sCom.pPub[i]->sFrame->sAck.resend_num++;  //接收超时次数
                }
            }
            else if(sCom.pHeart[i]->recordTime->curTime
                    - sCom.pHeart[i]->recordTime->lastTime
                    > sCom.pHeart[i]->recordTime->offsetTime)
            {
                sCom.pHeart[i]->recordTime->lastTime = getSysTime();
                sCom.pPub[i]->sFrame->sAck.resend_num++;  //接收超时次数
            }
        }

        if(sCom.pPub[i]->sFrame->sAck.resend_set_num !=0
                &&  sCom.pPub[i]->sFrame->sAck.resend_num >= sCom.pPub[i]->sFrame->sAck.resend_set_num)
        {
            if((sCom.pPub[i]->id_m == TX2_ID)&&(comlogflag.TX2_COMLOGFLAG))
            {
                if(!(tcp_constatus & 0x01))
                {
                    GR_LOG_ERROR("TX2 Disconnect,because there's no cable plugged in!");
                    m_comm_state.flag.b00_tx2_err = COM_STATE_ERR;
                }
                m_comm_state.flag.b00_tx2_err = COM_STATE_OK;
                comlogflag.TX2_COMLOGFLAG = 0;
            }
            if((sCom.pPub[i]->id_m == POW_ID)&&(comlogflag.POWER_COMLOGFLAG))
            {
                GR_LOG_ERROR("PowerBoard Disconnect!");
                comlogflag.POWER_COMLOGFLAG = 0;
            }
            if((sCom.pPub[i]->id_m == JOYS_ID)&&(comlogflag.JOY_COMLOGFLAG))
            {
                GR_LOG_ERROR("JOYBoard Disconnect!");
                comlogflag.JOY_COMLOGFLAG = 0;
            }
            if((sCom.pPub[i]->id_m == ULT_ID)&&(comlogflag.ULT_COMLOGFLAG))
            {
                GR_LOG_ERROR("ULTBoard Disconnect!");
                comlogflag.ULT_COMLOGFLAG = 0;
            }

            sCom.comState[sCom.pPub[i]->id_m] = 0;

#ifndef __COMMUN_USER_NET
            if(sCom.pPub[i]->id_m == 1)
            {
                Reco_flag = 1;
            }
#endif
            if(sCom.device_type == SLAVE_DEVICE)
            {
                sCom.pPub[i]->sFrame->sAck.resend_num =
                    sCom.pPub[i]->sFrame->sAck.resend_set_num;
            }
//			error_cnt++;
        }
#ifdef __COMMUN_USER_NET
        if((!(tcp_constatus & 0x01))||(Ir_status == 0))//add by hopehu
        {
            sCom.comState[1] = 0;
        }
#endif
    }
}

unsigned long int error_cnt = 0;

//unsigned long int t1, t2, t;

void handleTX2DisconnectEvent(void)
{
    if((getTX2CommunicationState() == 0) && (getChassisFsmCurState() != CHASSIS_STATE_FAULT))
    {
        sendEvtToChassisFsm(FSM_EVT_SYSTEM_FAULT);
        GR_LOG_INFO("fsm fault occur, because TX2 communication disconnect.");
    }
}

void DealAllReceiveData()
{
    // HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    updateReceiveDataCount();
    GetReceiveData();
    // HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
    for(int i=0; i<cal_lin_buffer_size(); i++)
    {
        receiveHanlder(lin_buffer_data[i]);
    }
}
void infoSpinOnceHanlder()
{
    DealAllReceiveData();

    if(sCom.device_type == MASTER_DEVICE)  //只有主设备才发送心跳检测周边设备
    {
        PublishHeartSingal(); //发送心跳信号
    }

    for(int i=0; i<cal_Publish_size() + cal_heartSingal_size(); i++)
    {
        waitAckData(sCom.pPub[i]->id_m,sCom.pPub[i]->id_s);       //等待ACK应答信号

    }
    setconnectedState();  //设置当前连接状态
    handleTX2DisconnectEvent();
}


unsigned char getJOYCommunicationState(void)
{
    //返回0 通信失败
    return sCom.comState[JOYS_ID];
}

unsigned char getBMSCommunicationState(void)
{
    //返回0 通信失败
    return sCom.comState[POW_ID];
}

unsigned char getTX2CommunicationState(void)
{
    //返回0 通信失败
    return sCom.comState[TX2_ID];
}

unsigned char getUltrasonicCommunicationState(void)
{
    //返回0 通信失败
    return sCom.comState[ULT_ID];
}

unsigned char getLightstripCommunicationState(void)
{
    //返回0 通信失败
    return sCom.comState[LED_ID];
}

unsigned long int getErrorCnt(void)
{
    return err_cnt;
}

/*************************************************
Function: 			getCommState
Description:		通信状态 老的告警接口
Input:					none
Output: 				none
Return: 				flag_
*************************************************/
uint8_t getCommState(void)
{
    return m_comm_state.u8_v;
}

/*************************************************
Function: 			getLedState
Description:		灯带通信状态
Input:					none
Output: 				none
*************************************************/
uint8_t getLedState(void)
{
    int ret = 0;
    static bool bLog = false;
    ret = getLightstripCommunicationState();
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP58_ledCommunWarn,ret,0);
#endif
    if(ret == 0)
    {
        //灯带模块通讯异常
        if(!bLog)
        {
            setAlarm(eA_EXTE,eb03_ledCommunWarn,ALARM_ERR);
            GR_LOG_WARN("led communaton error");
            bLog = true;
        }
        return ERR;
    }
    if(bLog)
		{
        GR_LOG_INFO("led communaton recover ");
				bLog = false;
			  setAlarm(eA_EXTE,eb03_ledCommunWarn,ALARM_OK);
		}

    return OK;
}



