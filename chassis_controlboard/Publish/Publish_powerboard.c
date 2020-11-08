#include "Publish_powerboard.h"
uint8_t Send_BmsInfo(void)
{
    static unsigned long int t = 0;
    getSystemTimeStamp(t++, &Battery_Pub_Buffer.time);
    Battery_Pub_Buffer.voltage = Bms_Info.total_volatage;
    Battery_Pub_Buffer.current = Bms_Info.current;
    Battery_Pub_Buffer.temperature = ((Bms_Info.temperature1 > Bms_Info.temperature2)? Bms_Info.temperature1 : Bms_Info.temperature2);
    Battery_Pub_Buffer.charge = Bms_Info.status;
    Battery_Pub_Buffer.capacity = Bms_Info.RemainingCapacity;
    Battery_Pub_Buffer.design_capacity = Bms_Info.FullChargeCapacity;
    Battery_Pub_Buffer.percentage = Bms_Info.RelativeStateOfCharge;

    publishFrameData(POW_ID,BATTERY_INFO_ID, (unsigned char *)&Battery_Pub_Buffer, sizeof(Battery_InfoTypeDef));
    return 1;
}

void Reboot_Power(uint8_t power_type,uint8_t power_status)
{
    unsigned char data[sizeof(Power_HandleTypeDef)];
    unsigned char id;
    id = 5;
//power reboot
    Power_Control_Buffer.ID_Code       = power_type;			  
    Power_Control_Buffer.Function_Code = power_status;			 
    memcpy(&data, &Power_Control_Buffer, sizeof(Power_HandleTypeDef));
    publishFrameData(POW_ID,id, data, sizeof(Power_HandleTypeDef));
}

