#include <stdio.h>
#include "buffer.h"
#include "stm32f4xx_hal.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"
#include "usb_driver.h"
#include "usart.h"
#include "tcp_demo.h"
#include "socket.h"
#include "chassis_config.h"
#include "Frame_statistical.h"
#include "info_core.h"
#include "usbd_desc.h"
#include "cmsis_os.h"
#include "SD_Log_App.h"
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif
#include "alarm_com.h"
#include "Communication.h"
#include "chassis_com.h"

BUFFER_DATA powerBoard_buffer;
BUFFER_DATA sensorBoard_buffer;
BUFFER_DATA TX2Board_buffer;
BUFFER_DATA ultrasonicBoard_buffer;
BUFFER_DATA LightstripBoard_buffer;

extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern osMutexId COM_BUSYHandle;
extern SPI_HandleTypeDef hspi1;
extern uint32_t rxd_BufPtrIn;
#if 0

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
};

FILE __stdout;
void _sys_exit(int x)
{
    x = x;
}

int fputc(int ch, FILE *f)
{
    return HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFFFF);

}

#endif
uint32_t Tx_Test;

void transmitTX2Data(int length,unsigned char *data)
{  
// 使用网口通信
#ifdef __COMMUN_USER_NET
		static bool bLog =false;
    osMutexWait(COM_BUSYHandle,osWaitForever);
#ifdef __TP_USER
    TP_MODIFY_VALUE(eTP16_tx2_heart_packet_loss,tcp_constatus,0);
#endif
 //  if((tcp_connectflag == 1)&&(tcp_constatus&0x01)&&(Ir_status != 0)&&(hspi1.Lock != HAL_LOCKED))
		if((tcp_connectflag == 1)&&(tcp_constatus&0x01)&&(Ir_status != 0))
    {
        MainBoard_SendFrame_Statistical(data);
        send(SOCK_TCPS,data,length);
        if(bLog)
        {
            GR_LOG_INFO("heart send packet loss recover !");
            bLog = false;
        }
        setAlarm(eA_SYS,eb19_heartPacketLossErr,ALARM_OK);
    }
    else
    {
        if(!bLog)
        {
            GR_LOG_ERROR("heart send packet loss");
            bLog = true;
						Tx_Test ++;
        }
        setAlarm(eA_SYS,eb19_heartPacketLossErr,ALARM_ERR);
    }
    osMutexRelease(COM_BUSYHandle);

#else
    // 使用USB通信
    if(sCom.comState[1] == 1)
    {
        MainBoard_SendFrame_Statistical(data);
    }
    Driver_USB_Itf_Write(data, length);
    Driver_USB_Itf_SofISR();

#endif
}

unsigned long int readSysTime_ms(void)
{
    return HAL_GetTick();
}

void transmitPowerBoardData(int length,unsigned char *data)
{
    //数据发送函数
    HAL_UART_Transmit(&huart3, data, length, 0xFFFF);
}

void transmitJoyBoardData(int length,unsigned char *data)
{
#ifndef __TEST_CASE_EN
    HAL_UART_Transmit(&huart1, data, length, 0xFFFF);
#endif
}

static uint8_t ult_reboot_falg = 0;
void set_ult_reboot( uint8_t flag)
{
	ult_reboot_falg = flag;
}

uint8_t get_ult_reboot()
{
		return ult_reboot_falg;
}
void transmitUltrasonicBoardData(int length,unsigned char *data)
{
		static uint32_t delay_time = 0;
	
		if((MainBoard_Online_Update_Status.Status == Update_Ok )
			|| (get_ult_reboot() == ON))
		{		
				delay_time = getSystemTick();
				MainBoard_Online_Update_Status.Status = ready_error;
        set_ult_reboot(OFF);
				return;
		}
		if(getSystemTick() > delay_time + 6000)
		{
				HAL_UART_Transmit(&huart4, data, length, 0xFFFF);
		}
}

void transmitLightstripBoardData(int length,unsigned char *data)
{
    HAL_UART_Transmit(&huart2, data, length, 0xFFFF);
}
void updateReceiveDataCount(void)
{
    powerBoard_buffer.RX_head  = UART_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
    sensorBoard_buffer.RX_head = UART_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
    ultrasonicBoard_buffer.RX_head = UART_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
    LightstripBoard_buffer.RX_head = UART_BUFSIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
}

#if 1
void receiveDataToRXbuffer(unsigned char data, BUFFER_DATA *buffer_data)  //在接收中断中调用
{
    int tempRX_Head = (buffer_data->RX_head + 1) % (UART_BUFSIZE-0);

    int tempRX_Tail = buffer_data->RX_tail;

    //接收数据
    if (tempRX_Head == tempRX_Tail)
    {
        //表示接收缓冲区已经满了
    }
    else
    {
        buffer_data->RX[buffer_data->RX_head] = data;
        buffer_data->RX_head = tempRX_Head;
    }
}

void GetReceiveData(void)
{

// 使用网口通信
#ifdef __COMMUN_USER_NET
    unsigned short len=0;
    uint8_t tcp_buffer[2048];
    len=getSn_RX_RSR(SOCK_TCPS);
    if(len>0)
    {
        if(len>sizeof(tcp_buffer))
        {
            len = sizeof(tcp_buffer);
        }

        recv(SOCK_TCPS,tcp_buffer,len);
        for(int i = 0; i<len; i++)
        {
            receiveDataToRXbuffer(tcp_buffer[i], &TX2Board_buffer);
        }
    }
#else
    // 使用USB通信
    uint32_t len, i;
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    if(TX2Board_buffer.RX_head == rxd_BufPtrIn)
        return;

    if(rxd_BufPtrIn > TX2Board_buffer.RX_head)
        len = rxd_BufPtrIn - TX2Board_buffer.RX_head;
    else
        len = TX2Board_buffer.RX_head - rxd_BufPtrIn;

    for(i = 0; i < len; i++)
    {
        if ( Driver_USB_Itf_Available() == 0 )
            break;

        receiveDataToRXbuffer(Driver_USB_Itf_Getch(), &TX2Board_buffer);
    }

    TX2Board_buffer.RX_head = rxd_BufPtrIn;
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
#endif

}
#endif


void info_delay_ms(unsigned long int time_data)
{
    HAL_Delay(time_data);
}
#if 0

char judgmentFreeSpace(BUFFER_DATA *buffer_data)
{
    int tempHead = (buffer_data->TX_head + 1) % (UART_BUFSIZE-1);
    int tempTail = buffer_data->TX_tail;
    return (tempHead != tempTail);
}

//将数据添加到发送缓冲区
char addPublishDatatoTXBuffer(int length,unsigned char *data,BUFFER_DATA *buffer_data)
{
    int tempTX_Head;
    char isFree;
    for(int i = 0; i<length; i++)
    {
        isFree = judgmentFreeSpace(buffer_data);

        if(isFree)
        {
            tempTX_Head = buffer_data->TX_head;

            //禁止中断
            buffer_data->TX[tempTX_Head]= data[i];
            buffer_data->TX_head = (tempTX_Head + 1) % (UART_BUFSIZE-0);
            //使能中断
        }
        else
        {
            break;
        }
    }
    return isFree;
}


//发送一个字节的数据
void publishData(BUFFER_DATA *buffer_data)
{
    int tempTX_Tail = buffer_data->TX_tail;
    if (buffer_data->TX_head == tempTX_Tail)
    {
        //表示发送缓冲区数据以空
    }
    else
    {
        unsigned char data = buffer_data->TX[buffer_data->TX_tail];
        //发送数据

        buffer_data->TX_tail = (buffer_data->TX_tail + 1) % (UART_BUFSIZE-0);
    }
}


//判断是否接收到数据
char judgmentDataAvailable(BUFFER_DATA *buffer_data)
{
    int tempHead = buffer_data->RX_head;
    int tempTail = buffer_data->RX_tail;
    /* There are data left in the buffer unless Head and Tail are equal. */
    return (tempHead != tempTail);
}

//从接收缓冲区中读取一字节数据
unsigned char readBufferData(BUFFER_DATA *buffer_data)
{
    unsigned char ans;

    //禁止中断
    ans = buffer_data->RX[buffer_data->RX_tail];
    buffer_data->RX_tail = (buffer_data->RX_tail + 1) % (UART_BUFSIZE-0);
    //使能中断

    return ans;
}
#endif
