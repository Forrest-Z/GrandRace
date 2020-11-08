#ifndef __BMS_INFO_H__
#define __BMS_INFO_H__

#include <stdint.h>
#pragma pack(1)

typedef struct {
    uint32_t  warning;
    uint32_t  status;//是否充电
    int32_t   temperature1;
    int32_t   temperature2;
    uint32_t  total_volatage;
    float    	current;
    uint32_t  RelativeStateOfCharge;
    uint32_t  RemainingCapacity;
    uint32_t  Last_Chg_Time;
    uint32_t  FullChargeCapacity;

    //Volatage_INFO
    uint8_t 	Volatage_In12V_Error_Flag;
    uint8_t		Volatage_TX2_Error_Flag;
    uint8_t		Volatage_5V_Error_Flag;
    uint8_t		Volatage_Out12V_Error_Flag;

    //Current_INFO
    uint8_t PAD_CURRENT_STATUS;
    uint8_t RTK_CURRENT_STATUS;
    uint8_t JOY_CURRENT_STATUS;
		uint8_t res; //预留
		//Powerboard Temp
		float powerboard_temp;
} Bms_Buffer;
#endif


