#ifndef __MBMASTER_H
#define __MBMASTER_H

#include "stm32f4xx_hal.h"
#pragma pack(1)
typedef enum
{
	Mb_NO_ERR=0U,
	Mb_ERR_CRC,
	Mb_ERR_ID,
	Mb_ERR_CMD,
	Mb_ERR_FRAME,
	Mb_ERR_TIMEOUT,
	Mb_ERR_Overflow,
} TYPE_MbBusErrorStatus;

typedef enum
{
	Mb_BUS_FREE=0U,
	Mb_BUS_BUSY,
} TYPE_MbBusStatus;

typedef enum
{
	Mb_READ=0x03,
	Mb_WRITE_SINGLE=0x06,
	Mb_WRITE_MULTIPLE=0x10,
	Mb_READ_ERRCODE=0x83,
	Mb_WRITE_SINGLE_ERRCODE=0x86,
	Mb_WRITE_MULTIPLE_ERRCODE=0x90,
} TYPE_MbCmd;

typedef enum
{
	ErrorCode_NoError=0x00,
	ErrorCode_IllegalFunction,
	ErrorCode_IllegalAddr,
	ErrorCode_IllegalData,
	ErrorCode_SlaverFault,
	ErrorCode_CannotReply,
	ErrorCode_SlaverBusy,
	ErrorCode_OddEvenError,
	ErrorCode_CannotUsed,
	ErrorCode_CannotAck,
	ErrorCode_ForbidOperate=0x40,
	ErrorCode_NoStudy,
	ErrorCode_UnknownError,
} TYPE_ModbusErrorCode;

typedef struct
{
	TYPE_MbCmd Cmd;						//Modbus命令 
	uint8_t  	 DeviceID;			//从机地址
	uint16_t	 TPTR;					//要读或者写的首地址
	uint16_t 	 Lenth;					//读或写的数据长度
	uint8_t  	 *Buff;					//要写的数据首地址或读取数据的保存首地址
	uint8_t    Timeoutdata;		//超时时间
} TYPE_MbComunication;

#define Mb_QUEUE_MAX_LENTH 100
#define Mb_RECE__MAX_LENTH 100
typedef struct
{
	TYPE_MbComunication    Queue[Mb_QUEUE_MAX_LENTH];					//Modbus 执行队列
	uint16_t Real_Addr;																        //Modbus 执行的位置
	uint16_t End_Addr;																        //Modbus 队列末尾位置
	TYPE_MbBusStatus			 BusStatus;									        //Modbus 占用状态
	TYPE_MbBusErrorStatus	 ErrorStatus;								        //Modbus 错误状态
	TYPE_ModbusErrorCode   ModbusErrorCode;										//Modbus 错误码                                                   
	ErrorStatus						 ReceiveStatus;							        //Modbus 接收状态__(需要用户在接收完成函数中标记为SUCCESS)
	uint16_t							 ReceiveLenth;							        //Modbus 接收长度__(需要用户在接收完成函数中判别)
	uint8_t                ReceiveData[Mb_RECE__MAX_LENTH];		//Modbus 接收到的原始数据__(需要用户把接收到的数据储存在这里)
} TYPE_Mb;
void Mb_SendData(uint8_t *buff,uint16_t len);
extern TYPE_Mb ModbusRTU;

//注册modbusRTU发送函数
extern void Mb_Init(void (*SendData)(uint8_t *buff,uint16_t len));

extern void Mb_Read_holdingREG(TYPE_MbComunication Mb_Read);
extern void Mb_Write_Single_holdingREG(TYPE_MbComunication Mb_Write);
extern void Mb_Write_Multiple_holdingREG(TYPE_MbComunication Mb_Write);
extern TYPE_MbBusErrorStatus Mb_Responses(TYPE_MbComunication Mb_DATA);

extern ErrorStatus Mb_Poll(TYPE_MbComunication Mb_DATA);		//用户写入数据到队列
extern TYPE_MbBusErrorStatus Mb_Working(void);			
extern void Mb_Time1ms(void);
#endif

