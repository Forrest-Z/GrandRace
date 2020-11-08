/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WIZnet Software Team
* @version 		V1.0
* @date    		2015-02-14
* @brief  		配置MCU，移植W5500程序需要修改的文件，配置W5500的MAC和IP地址
**************************************************************************************************
*/
#include <stdio.h>
#include <string.h>

#include "w5500_conf.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "spi.h"
#include "gpio.h"
#include "cmsis_os.h"

CONFIG_MSG  ConfigMsg;																	/*配置结构体*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM存储信息结构体*/
SPI_HandleTypeDef hspi_w5500;

extern osMutexId TCP_busyHandle;
uint8_t w5500_init_flag;

/*定义MAC地址,如果多块W5500网络适配板在同一现场工作，请使用不同的MAC地址*/
uint8 mac[6]= {0x00,0x08,0xdc,0x11,0x11,0x11};
uint8_t get_mac[6] = {0};
/*定义默认IP信息*/
uint8 local_ip[4]  = {192,168,2,11};										/*定义W5500默认IP地址192.168.2.11*/
uint8_t get_local_ip[4] = {0};
uint8 subnet[4]    = {255,255,255,0};										/*定义W5500默认子网掩码*/
uint8_t get_subnet[4] = {0};
uint8 gateway[4]   = {192,168,2,1};											/*定义W5500默认网关*/
uint8_t get_gateway[4] = {0};
uint8 dns_server[4]= {114,114,114,114};									/*定义W5500默认DNS*/

uint16 local_port=5000;	                       					/*定义本地端口*/

/*定义远端IP信息*/
uint8  remote_ip[4]= {192,168,2,12};			  						/*远端IP地址*/
uint16 remote_port=8080;																/*远端端口号*/

/*IP配置方法选择，请自行选择*/
uint8	ip_from=IP_FROM_DEFINE;

uint8         dhcp_ok   = 0;													   			/*dhcp成功获取IP*/
uint32	      ms        = 0;															  	/*毫秒计数*/
uint32	      dhcp_time = 0;															  	/*DHCP运行计数*/
__IO uint8_t  ntptimer  = 0;															  	/*NPT秒计数*/

/**
*@brief		硬件复位W5500
*@param		无
*@return	无
*/
void reset_w5500(void)
{
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin,GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin,GPIO_PIN_SET);
    HAL_Delay(1600);
}

/**
*@brief		配置W5500的IP地址
*@param		无
*@return	无
*/
uint8_t set_w5500_ip(void)
{
    /*复制定义的配置信息到配置结构体*/
    memcpy(ConfigMsg.mac, mac, 6);
    memcpy(ConfigMsg.lip,local_ip,4);
    memcpy(ConfigMsg.sub,subnet,4);
    memcpy(ConfigMsg.gw,gateway,4);
    memcpy(ConfigMsg.dns,dns_server,4);
    if(ip_from==IP_FROM_DEFINE)
    {
        //printf(" 使用定义的IP信息配置W5500\r\n");
    }
    /*以下配置信息，根据需要选用*/
    ConfigMsg.sw_ver[0]=FW_VER_HIGH;
    ConfigMsg.sw_ver[1]=FW_VER_LOW;

    /*将IP配置信息写入W5500相应寄存器*/
    setSUBR(ConfigMsg.sub);
    setGAR(ConfigMsg.gw);
    setSIPR(ConfigMsg.lip);

    getSIPR (get_local_ip);
    for(int i = 0;i<4;i++)
		{
			if(get_local_ip[i] != local_ip[i])
			{
				return 0;
			}
		}
    getSUBR(get_subnet);
		for(int i = 0;i<4;i++)
		{
			if(get_subnet[i] != subnet[i])
			{
				return 0;
			}
		}
    getGAR(get_gateway);
		for(int i = 0;i<4;i++)
		{
			if(get_gateway[i] != gateway[i])
			{
				return 0;
			}
		}
		return 1;
}

/**
*@brief		配置W5500的MAC地址
*@param		无
*@return	无
*/
uint8_t set_w5500_mac(void)
{
    memcpy(ConfigMsg.mac, mac, 6);
    setSHAR(ConfigMsg.mac);	/**/
		getSHAR(get_mac);
    for(int i = 0;i<6;i++)
		{
			if(mac[i] != get_mac[i])
			{
				return 0;
			}
		}
		return 1;
}

/**
*@brief		W5500片选信号设置函数
*@param		val: 为“0”表示片选端口为低，为“1”表示片选端口为高
*@return	无
*/
void wiz_cs(uint8_t val)
{
    if (val == LOW)
    {
        HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_RESET);
    }
    else if (val == HIGH)
    {
        HAL_GPIO_WritePin(W5500_NSS_GPIO_Port, W5500_NSS_Pin, GPIO_PIN_SET);
    }
}

/**
*@brief		设置W5500的片选端口SCSn为低
*@param		无
*@return	无
*/
void iinchip_csoff(void)
{
    wiz_cs(LOW);
}

/**
*@brief		设置W5500的片选端口SCSn为高
*@param		无
*@return	无
*/
void iinchip_cson(void)
{
    wiz_cs(HIGH);
}

uint8_t SPI_SendByte(uint8_t byte)
{
    uint8_t d_read,d_send=byte;
    if(HAL_SPI_TransmitReceive(&hspi1,&d_send,&d_read,1,0xFFFFFF)!=HAL_OK)
        d_read=0XFF;

    return d_read;
}

/**
*@brief		STM32 SPI1读写8位数据
*@param		dat：写入的8位数据
*@return	无
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
    return(SPI_SendByte(dat));
}

/**
*@brief		写入一个8位数据到W5500
*@param		addrbsb: 写入数据的地址
*@param   data：写入的8位数据
*@return	无
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
		if(w5500_init_flag == 0xFF)
			osMutexWait(TCP_busyHandle,osWaitForever);
	
    iinchip_csoff();
    IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
    IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
    IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);
    IINCHIP_SpiSendData(data);
    iinchip_cson();
	
		if(w5500_init_flag == 0xFF)
			osMutexRelease(TCP_busyHandle);
}

/**
*@brief		从W5500读出一个8位数据
*@param		addrbsb: 写入数据的地址
*@param   data：从写入的地址处读取到的8位数据
*@return	无
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
    uint8 data = 0;
	
	if(w5500_init_flag == 0xFF)
		osMutexWait(TCP_busyHandle,osWaitForever);
	
    iinchip_csoff();
    IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
    IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
    IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
    data = IINCHIP_SpiSendData(0x00);
    iinchip_cson();
	
	if(w5500_init_flag == 0xFF)
		osMutexRelease(TCP_busyHandle);
	
    return data;
}

/**
*@brief		向W5500写入len字节数据
*@param		addrbsb: 写入数据的地址
*@param   buf：写入字符串
*@param   len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
    uint16 idx = 0;
    if(len == 0)
    {
        //printf("Unexpected2 length 0\r\n");
    }
		
		if(w5500_init_flag == 0xFF)
			osMutexWait(TCP_busyHandle,osWaitForever);
		
    iinchip_csoff();
    IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
    IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
    IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);
    for(idx = 0; idx < len; idx++)
    {
        IINCHIP_SpiSendData(buf[idx]);
    }
    iinchip_cson();
		
		if(w5500_init_flag == 0xFF)
			osMutexRelease(TCP_busyHandle);
		
    return len;
}

/**
*@brief		从W5500读出len字节数据
*@param		addrbsb: 读取数据的地址
*@param 	buf：存放读取数据
*@param		len：字符串长度
*@return	len：返回字符串长度
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
    uint16 idx = 0;
    if(len == 0)
    {
        //printf("Unexpected2 length 0\r\n");
    }
		
		if(w5500_init_flag == 0xFF)
			osMutexWait(TCP_busyHandle,osWaitForever);
		
    iinchip_csoff();
    IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
    IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
    IINCHIP_SpiSendData( (addrbsb & 0x000000F8));
    for(idx = 0; idx < len; idx++)
    {
        buf[idx] = IINCHIP_SpiSendData(0x00);
    }
    iinchip_cson();
		
		if(w5500_init_flag == 0xFF)
			osMutexRelease(TCP_busyHandle);
		
    return len;
}
