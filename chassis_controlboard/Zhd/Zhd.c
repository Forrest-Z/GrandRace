#include "Zhd.h"

unsigned char zhd_sndshftpos(void)
{
	CAN_TxHeaderTypeDef  TxMessage;
  uint8_t buf[8] = {0,0,0,0,0,0,0,0};
	
	TxMessage.DLC=8;
	TxMessage.StdId=0x1F5;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	
	if((controlDataInst.present_v[0]+controlDataInst.present_v[1]) > 0.1f)
	{
	  buf[3] = 0x04;//前进
	}
	else if((controlDataInst.present_v[0]+controlDataInst.present_v[1]) < -0.1f)
	{
		buf[3] = 0x02;//后退
	}
	else if((fabs(controlDataInst.present_v[0]) < 0.1f)&&(fabs(controlDataInst.present_v[1]) < 0.1f))
	{
		buf[3] = 0x01;//停车
	}
 	
	if(HAL_CAN_AddTxMessage(&hcan1,&TxMessage,buf,(uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
	{
		HAL_CAN_Stop(&hcan1);
		HAL_CAN_Start(&hcan1);
		return 0;
	}
	return 1;
}

unsigned char zhd_sndVehSpdF(void)//前轮速度
{
	CAN_TxHeaderTypeDef  TxMessage;
  uint8_t buf[8];
	short speed[2];
	
	TxMessage.DLC=8;
	TxMessage.StdId=0x348;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	
	speed[0] = controlDataInst.present_v[0]*3.6f/0.03258f;
	speed[1] = controlDataInst.present_v[1]*3.6f/0.03258f;

	memset(buf,0,sizeof(buf));
	buf[0] = speed[0]>>8;
	buf[1] = (unsigned char)speed[0];
	buf[2] = speed[1]>>8;
	buf[3] = (unsigned char)speed[1];
	
	if(HAL_CAN_AddTxMessage(&hcan1,&TxMessage,buf,(uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
	{
		HAL_CAN_Stop(&hcan1);
		HAL_CAN_Start(&hcan1);
		return 0;
	}
	return 1;
}

unsigned char zhd_sndVehSpdR(void)//后轮速度
{
	CAN_TxHeaderTypeDef  TxMessage;
  uint8_t buf[8];
	short speed[2];
	
	TxMessage.DLC=8;
	TxMessage.StdId=0x34A;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.RTR=CAN_RTR_DATA;
	
	speed[0] = controlDataInst.present_v[0]*3.6f/0.03258f;
	speed[1] = controlDataInst.present_v[1]*3.6f/0.03258f;

	memset(buf,0,sizeof(buf));
	buf[0] = speed[0]>>8;
	buf[1] = (unsigned char)speed[0];
	buf[2] = speed[1]>>8;
	buf[3] = (unsigned char)speed[1];
	
	
	if(HAL_CAN_AddTxMessage(&hcan1,&TxMessage,buf,(uint32_t*)CAN_TX_MAILBOX1) != HAL_OK)
	{
		HAL_CAN_Stop(&hcan1);
		HAL_CAN_Start(&hcan1);
		return 0;
	}
	return 1;
}
