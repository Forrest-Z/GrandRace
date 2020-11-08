#include	"Mb_Master.h"
#include  "Mb_crc16.h"
#include <string.h>
#include "usart.h"
#include "cmsis_os.h"
static _Bool Real_Symbol=0;	//当前队列执行地址
static _Bool End_Symbol=0;	//有效队列末尾地址
static uint8_t  ModbusRTU_send_buff[100];

uint8_t Mb_time=0;
TYPE_Mb ModbusRTU;

void Mb_SendData(uint8_t *buff,uint16_t len)
{
   HAL_UART_Transmit_DMA(&huart6,buff,len);
}

//读保持寄存器（03）
void Mb_Read_holdingREG(TYPE_MbComunication Mb_Read)
{
	uint16_t CRC16_Check;

	ModbusRTU_send_buff[0]=Mb_Read.DeviceID;
	ModbusRTU_send_buff[1]=0X03;
	ModbusRTU_send_buff[2]=Mb_Read.TPTR>>8;
	ModbusRTU_send_buff[3]=Mb_Read.TPTR;
	ModbusRTU_send_buff[4]=Mb_Read.Lenth>>8;
	ModbusRTU_send_buff[5]=Mb_Read.Lenth;
	CRC16_Check=Modbus_CRC16(ModbusRTU_send_buff,6);
	ModbusRTU_send_buff[6]=CRC16_Check;
	ModbusRTU_send_buff[7]=CRC16_Check>>8;
	Mb_SendData(ModbusRTU_send_buff,8);
}
//发送写单个保持寄存器（06）
void Mb_Write_Single_holdingREG(TYPE_MbComunication Mb_Write)
{
	uint16_t CRC_Check;
	ModbusRTU_send_buff[0]=Mb_Write.DeviceID;
	ModbusRTU_send_buff[1]=0x06;
	ModbusRTU_send_buff[2]=Mb_Write.TPTR>>8;
	ModbusRTU_send_buff[3]=Mb_Write.TPTR;
	ModbusRTU_send_buff[4]=*(Mb_Write.Buff+1);
	ModbusRTU_send_buff[5]=*Mb_Write.Buff;
	CRC_Check=Modbus_CRC16(ModbusRTU_send_buff,6);
	ModbusRTU_send_buff[6]=CRC_Check;
	ModbusRTU_send_buff[7]=CRC_Check>>8;
	Mb_SendData(ModbusRTU_send_buff,8);
}
//发送写多个保持寄存器（16）
void Mb_Write_Multiple_holdingREG(TYPE_MbComunication Mb_Write)
{
	uint8_t i;
	uint16_t CRC_Check;
	ModbusRTU_send_buff[0]=Mb_Write.DeviceID;
	ModbusRTU_send_buff[1]=0x10;
	ModbusRTU_send_buff[2]=Mb_Write.TPTR>>8;
	ModbusRTU_send_buff[3]=Mb_Write.TPTR;
	ModbusRTU_send_buff[4]=Mb_Write.Lenth>>8;
	ModbusRTU_send_buff[5]=Mb_Write.Lenth;
	ModbusRTU_send_buff[6]=Mb_Write.Lenth<<1;
	
	for(i=0;i<ModbusRTU_send_buff[6];i++)
	{
		if(i%2==0)
			ModbusRTU_send_buff[7+i]=*(Mb_Write.Buff+i+1);
		else
			ModbusRTU_send_buff[7+i]=*(Mb_Write.Buff+i-1);
	}
	
	CRC_Check=Modbus_CRC16(ModbusRTU_send_buff,7+i);
	
	ModbusRTU_send_buff[7+i]=CRC_Check;
	ModbusRTU_send_buff[8+i]=CRC_Check>>8;
	
	Mb_SendData(ModbusRTU_send_buff,9+i);
}

//Modbus执行任务
TYPE_MbBusErrorStatus Mb_Working(void)
{
	static uint16_t read_addr=0;
//**************************************************	
//接收数据
//**************************************************
//查询总线是否被暂用
	if(ModbusRTU.BusStatus!=Mb_BUS_FREE)
	{
		if(ModbusRTU.Real_Addr==0)
			read_addr=Mb_QUEUE_MAX_LENTH-1;
		else
			read_addr=ModbusRTU.Real_Addr-1;
//总线已经成功接收到数据或者已经达到最大超时时间
		if(ModbusRTU.ReceiveStatus==SUCCESS||Mb_time>ModbusRTU.Queue[read_addr].Timeoutdata)
		{
			HAL_UART_Receive_DMA(&huart6,ModbusRTU.ReceiveData,Mb_RECE__MAX_LENTH);
			if(ModbusRTU.ReceiveStatus==SUCCESS)
			{			
				ModbusRTU.ErrorStatus=Mb_Responses(ModbusRTU.Queue[read_addr]);
				ModbusRTU.BusStatus=Mb_BUS_FREE;				//释放总线
				ModbusRTU.ReceiveStatus=ERROR;					//标记为无接收状态
			}
			else
			{
				ModbusRTU.ErrorStatus=Mb_ERR_TIMEOUT;		//显示超时
				ModbusRTU.BusStatus=Mb_BUS_FREE;				//释放总线
			}
			//如果上次发送的数据错误或超时，则本次重发
			if(ModbusRTU.ErrorStatus!=Mb_NO_ERR)
			{
				static uint8_t Time_Delay = 0;
				if(Time_Delay<10||Time_Delay == 10)
				{	
					Time_Delay++;
				}
				else if(Time_Delay>10)
				{	
					ModbusRTU.Real_Addr=read_addr;
					ModbusRTU.BusStatus=Mb_BUS_FREE;				//释放总线
				  Time_Delay = 0;
				}
			}
		}
	}
//**************************************************
//发送数据
//总线为空闲的时候才可以发送数据
//**************************************************	
	if(ModbusRTU.BusStatus==Mb_BUS_FREE)
	{
//判断队列数据是否发送完毕,若已经完成则直接退出
		if(Real_Symbol==End_Symbol)
		{
			if(ModbusRTU.Real_Addr==ModbusRTU.End_Addr)
				return Mb_NO_ERR;
		}	
//发送读指令
		if(ModbusRTU.Queue[ModbusRTU.Real_Addr].Cmd==Mb_READ)
		{
			Mb_Read_holdingREG(ModbusRTU.Queue[ModbusRTU.Real_Addr]);
			Mb_time=0;
			ModbusRTU.BusStatus=Mb_BUS_BUSY;				//占用总线
		}
//发送写指令	
		else if(ModbusRTU.Queue[ModbusRTU.Real_Addr].Cmd==Mb_WRITE_SINGLE)
		{
			Mb_Write_Single_holdingREG(ModbusRTU.Queue[ModbusRTU.Real_Addr]);
			Mb_time=0;
			ModbusRTU.BusStatus=Mb_BUS_BUSY;				//占用总线
		}
		else
		{
			Mb_Write_Multiple_holdingREG(ModbusRTU.Queue[ModbusRTU.Real_Addr]);
			Mb_time=0;
			ModbusRTU.BusStatus=Mb_BUS_BUSY;				//占用总线
		}
	//轮询地址增加
		if(ModbusRTU.Real_Addr<Mb_QUEUE_MAX_LENTH-1)
		{
			ModbusRTU.Real_Addr++;
		}
		else
		{
			ModbusRTU.Real_Addr=0;
			Real_Symbol=~Real_Symbol;
		}
	}
	return Mb_NO_ERR;
}

//接收处理函数
TYPE_MbBusErrorStatus Mb_Responses(TYPE_MbComunication Mb_DATA)
{
	uint16_t i;
	uint16_t CRC_Check;
//接收长度异常
	if(ModbusRTU.ReceiveLenth<2) 
		return Mb_ERR_FRAME;
//CRC校验异常
	CRC_Check=ModbusRTU.ReceiveData[ModbusRTU.ReceiveLenth-2]+ModbusRTU.ReceiveData[ModbusRTU.ReceiveLenth-1]*256;
	if(CRC_Check==Modbus_CRC16(ModbusRTU.ReceiveData,ModbusRTU.ReceiveLenth-2))
	{
//判断从机地址是否正确
		if(Mb_DATA.DeviceID!=ModbusRTU.ReceiveData[0])
			return Mb_ERR_ID;
//判断Modbus错误码
		if(ModbusRTU.ReceiveData[1]==Mb_READ_ERRCODE||ModbusRTU.ReceiveData[1]==Mb_WRITE_SINGLE_ERRCODE||ModbusRTU.ReceiveData[1]==Mb_WRITE_MULTIPLE_ERRCODE)
		{
			ModbusRTU.ModbusErrorCode=(TYPE_ModbusErrorCode)ModbusRTU.ReceiveData[2];
			return Mb_NO_ERR;
		}
//		else
//		{
//			ModbusRTU.ModbusErrorCode=ErrorCode_NoError;
//		}
//判断读写指令是否正确
		if(Mb_DATA.Cmd!=ModbusRTU.ReceiveData[1])
			return Mb_ERR_CMD;
		switch((TYPE_MbCmd)ModbusRTU.ReceiveData[1])
		{
			case Mb_READ:						memcpy(Mb_DATA.Buff,&ModbusRTU.ReceiveData[3],ModbusRTU.ReceiveData[2]);
															for(i=0;i<ModbusRTU.ReceiveData[2];i++)
															{
																if(i%2==0)
																	*(Mb_DATA.Buff+i)=ModbusRTU.ReceiveData[3+i+1];
																else
																	*(Mb_DATA.Buff+i)=ModbusRTU.ReceiveData[3+i-1];
															}
															return Mb_NO_ERR;
			case Mb_WRITE_SINGLE:		
															return Mb_NO_ERR;
			case Mb_WRITE_MULTIPLE:	
															return Mb_NO_ERR;
															
			default:
															return Mb_ERR_FRAME;
		}
	}
	return Mb_ERR_CRC;
}

ErrorStatus Mb_Poll(TYPE_MbComunication Mb_DATA)
{
	int16_t Delta;
//判断real和end的差值
	if(Real_Symbol==End_Symbol)
	{
		Delta=ModbusRTU.End_Addr-ModbusRTU.Real_Addr;
	}
	else
		Delta=Mb_QUEUE_MAX_LENTH+ModbusRTU.End_Addr-ModbusRTU.Real_Addr;
	if(Delta<0)
		Delta=-Delta;
	if(Delta>=Mb_QUEUE_MAX_LENTH-2)
	{
		return ERROR;
	}
	ModbusRTU.Queue[ModbusRTU.End_Addr].DeviceID   =Mb_DATA.DeviceID;
	ModbusRTU.Queue[ModbusRTU.End_Addr].Cmd        =Mb_DATA.Cmd     ;
	ModbusRTU.Queue[ModbusRTU.End_Addr].Lenth      =Mb_DATA.Lenth   ;
	ModbusRTU.Queue[ModbusRTU.End_Addr].TPTR   		 =Mb_DATA.TPTR    ;
	ModbusRTU.Queue[ModbusRTU.End_Addr].Buff    	 =Mb_DATA.Buff    ;
	ModbusRTU.Queue[ModbusRTU.End_Addr].Timeoutdata=Mb_DATA.Timeoutdata;
	if(ModbusRTU.End_Addr<Mb_QUEUE_MAX_LENTH-1)
	{
		ModbusRTU.End_Addr++;
	}
	else
	{
		ModbusRTU.End_Addr=0;
		End_Symbol=~End_Symbol;
	}
	return SUCCESS;
}
void Mb_Time1ms(void)
{
	if(ModbusRTU.BusStatus!=Mb_BUS_FREE)
	{
		Mb_time++;
	}
}







