#include "Communication.h"
#include "OD.h"
MainBoard_Online_Update MainBoard_Online_Update_Status = {
    1,
    0,
    0,
};
MainBoard_Online_Update PowerBoard_Online_Update_Status = {
    2,
    0,
    0,
};
MainBoard_Online_Update SensorBoard_Online_Update_Status = {
    3,
    0,
    0,
};
MainBoard_Online_Update UltBoard_Online_Update_Status = {
    4,
    0,
    0,
};
MainBoard_Online_Update LightBoard_Online_Update_Status = {
    5,
    0,
    0,
};
Receive_Control_Buffer Receive_407_Buffer = {

    0,
    0,
};

void Init_Communication(void)
{

}
void Send_Mcu_Buffer(uint8_t buffer_type)
{
    if(buffer_type == ready_ok)
    {
        MainBoard_Online_Update_Status.Status = ready_ok;
        MainBoard_Online_Update_Status.board_type = 1;
        MainBoard_Online_Update_Status.num = 0;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == ready_error)
    {
        MainBoard_Online_Update_Status.Status = ready_error;
        MainBoard_Online_Update_Status.board_type = 1;
        MainBoard_Online_Update_Status.num = 0;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == recivce_ok)
    {
        MainBoard_Online_Update_Status.Status = recivce_ok;
        MainBoard_Online_Update_Status.board_type = 1;
        MainBoard_Online_Update_Status.num++;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == recivce_error)
    {
        MainBoard_Online_Update_Status.Status = recivce_error;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == Update_Ok)
    {
        MainBoard_Online_Update_Status.Status = Update_Ok;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == Update_Error)
    {
        MainBoard_Online_Update_Status.Status = Update_Error;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == Same_Volid)
    {
        MainBoard_Online_Update_Status.Status = Same_Volid;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == No_Valid_Code)
    {
        MainBoard_Online_Update_Status.Status = No_Valid_Code;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
    else if(buffer_type == last_recivce_ok)
    {
        MainBoard_Online_Update_Status.Status = recivce_ok;
        MainBoard_Online_Update_Status.board_type = 1;
        unsigned char data[sizeof(MainBoard_Online_Update)];
        memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
        publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
    }
}
void down_send(uint8_t board_type)
{
    if(board_type == POW_ID)
    {
        unsigned char data[sizeof(down_send_buffer_)];
        memcpy(&down_send_powerboard,&receive_buffer,sizeof(down_send_buffer_));
        memcpy(&data,&down_send_powerboard,sizeof(down_send_buffer_));
        publishFrameData(POW_ID,power_down_send_ID,data,sizeof(down_send_buffer_));
    }
    else if(board_type == JOYS_ID)
    {
        unsigned char data[sizeof(down_send_buffer_)];
        memcpy(&down_send_sensorboard,&receive_buffer,sizeof(down_send_buffer_));
        memcpy(&data,&down_send_sensorboard,sizeof(down_send_buffer_));
        publishFrameData(JOYS_ID,joy_down_send_ID,data,sizeof(down_send_buffer_));
    }
		else if(board_type == ULT_ID)
    {
        unsigned char data[sizeof(down_send_buffer_)];
        memcpy(&down_send_ultboard,&receive_buffer,sizeof(down_send_buffer_));
        memcpy(&data,&down_send_ultboard,sizeof(down_send_buffer_));
        publishFrameData(ULT_ID,PUB_ULT_UPDATE_ID,data,sizeof(down_send_buffer_));
    }
		else if(board_type == LED_ID)
    {
        unsigned char data[sizeof(down_send_buffer_)];
        memcpy(&down_send_lightboard,&receive_buffer,sizeof(down_send_buffer_));
        memcpy(&data,&down_send_lightboard,sizeof(down_send_buffer_));
        publishFrameData(LED_ID,Led_down_send_ID,data,sizeof(down_send_buffer_));
    }
}
