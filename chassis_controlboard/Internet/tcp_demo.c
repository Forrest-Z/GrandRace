#include <stdio.h>
#include <string.h>
#include "tcp_demo.h"
#include "W5500_conf.h"
#include "w5500.h"
#include "socket.h"
//uint8 buff[2048];				                              	         /*定义一个2KB的缓存*/
#include "OD.h"
#include "Publish_Pc.h"
#include "Frame_statistical.h"

#include "chassis_config.h"

#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif


uint8_t tcp_connectflag = 0;
uint16_t tcp_time_delay = 0;
uint8_t time_run_test = 0;
void do_tcp_server(void)
{
//	uint16 len=0;
    uint8 sock_status =getSn_SR(SOCK_TCPS);
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP15_comnun_disable, sock_status, SOCK_CLOSED)
#endif
    switch(sock_status)											            	/*获取socket的状态*/
    {
    case SOCK_CLOSED:													                  /*socket处于关闭状态*/
        tcp_connectflag = 0;
        socket(SOCK_TCPS,Sn_MR_TCP,local_port,Sn_MR_ND);	         /*打开socket*/
        break;

    case SOCK_INIT:														                  /*socket已初始化状态*/
        listen(SOCK_TCPS);												                /*socket建立监听*/
        break;

    case SOCK_ESTABLISHED:												              /*socket处于连接建立状态*/
        if(tcp_connectflag == 0)
        {
            statistics_contotx2_msg.save_flag = 1;
            statistics_tx2tocon_msg.save_flag = 1;

            if((contotx2_buf == NULL)&&(tx2tocon_buf == NULL))
            {
                Cache_contotx2_framebuffer(&statistics_contotx2_msg);
                Cache_tx2tocon_framebuffer(&statistics_tx2tocon_msg);
                time_run_test++;
            }
            memset(&statistics_contotx2_msg,0,sizeof(STATISTICS_CONTOTX2_MSG));
            memset(&statistics_tx2tocon_msg,0,sizeof(STATISTICS_TX2toCON_msg));
						statistics_tx2tocon_msg.sendTimesignal = 1;
						statistics_tx2tocon_msg.frame_total = 1;
            tcp_time_delay++;
            if(tcp_time_delay>100)
            {
                tcp_connectflag = 1;
                tcp_time_delay = 0;
            }
        }
        if(getSn_IR(SOCK_TCPS) & Sn_IR_CON)
        {
            setSn_IR(SOCK_TCPS, Sn_IR_CON);								          /*清除接收中断标志位*/
        }
//			len=getSn_RX_RSR(SOCK_TCPS);									            /*定义len为已接收数据的长度*/
//			if(len>0)
//			{
//				recv(SOCK_TCPS,buff,len);								              	/*接收来自Client的数据*/
//				buff[len]=0x00; 											                  /*添加字符串结束符*/
//				//printf("%s\r\n",buff);
//				//send(SOCK_TCPS,buff,len);									              /*向Client发送数据*/
//		  }
        break;

    case SOCK_CLOSE_WAIT:												                /*socket处于等待关闭状态*/
        close(SOCK_TCPS);
        break;
    }
}

