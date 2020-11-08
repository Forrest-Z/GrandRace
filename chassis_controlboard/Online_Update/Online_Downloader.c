#include "Online_Downloader.h"
#include "OD.h"
#include "watchdog_task.h"
//#include "monitor.h"
//#include "motor_driver.h"
#include "Motor_Control.h"


void Online_Update_Callback(void)
{
    if(!getParkingState())
    {
        return;
    }

    if(receive_buffer.board_type == 1)
    {
        if(receive_buffer.update_cmd == init_flash)
        {
            uint32_t SectorError;
            FLASH_EraseInitTypeDef pEraseInit;
            FLASH_If_Init();

            /* Get the sector where start the user flash area */
            pEraseInit.TypeErase = TYPEERASE_SECTORS;
            pEraseInit.Sector    = FLASH_SECTOR_8;
            pEraseInit.NbSectors = 4;
            pEraseInit.VoltageRange = VOLTAGE_RANGE_3;

            Watchdog_SectorErase_Reconfigure();
            __disable_irq();

            if(HAL_FLASHEx_Erase(&pEraseInit, &SectorError) == HAL_OK)
            {
                __enable_irq();
                Watchdog_Task_Start();
                HAL_FLASH_Lock();
                Send_Mcu_Buffer(ready_ok);
            }
            else
            {
                __enable_irq();
                Watchdog_Task_Start();
                HAL_FLASH_Lock();
                Send_Mcu_Buffer(ready_error);
            }
        }
        else if(receive_buffer.update_cmd == send_begin)
        {
            if((receive_buffer.num-1)==MainBoard_Online_Update_Status.num)
            {
                uint8_t bin_buffer[write_length];
                FLASH_If_Init();
                for(int i =0; i<write_length; i++)
                {
                    bin_buffer[i] = receive_buffer.buffer[i];
                }
                uint8_t * des_path = bin_buffer;
                if(FLASH_If_Write( APPLICATION_BUFFER_ADDRESS + (receive_buffer.num*write_length) - write_length, (uint32_t*)des_path, write_length/4 ) == FLASHIF_OK)
                {
                    HAL_FLASH_Lock();
                    Send_Mcu_Buffer(recivce_ok);
                }
                else
                {
                    Send_Mcu_Buffer(recivce_error);
                    HAL_FLASH_Lock();
                }
            }
            else if(((receive_buffer.num+1)==MainBoard_Online_Update_Status.num)||((receive_buffer.num==MainBoard_Online_Update_Status.num)))
            {
                Send_Mcu_Buffer(last_recivce_ok);
            }
        }
        else if(receive_buffer.update_cmd == send_end)
        {
            Watchdog_SectorErase_Reconfigure();
            check_buffer();
            Watchdog_Task_Start();
        }
    }
    else if(receive_buffer.board_type == POW_ID)
    {
        down_send(POW_ID);
    }
    else if(receive_buffer.board_type == JOYS_ID)
    {
        down_send(JOYS_ID);
    }
		else if(receive_buffer.board_type == ULT_ID)
    {
        down_send(ULT_ID);
    }
		else if(receive_buffer.board_type == LED_ID)
    {
        down_send(LED_ID);
    }
}
void PowerBoard_Online_Update_Callback(void)
{
    unsigned char data[sizeof(MainBoard_Online_Update)];
    memcpy(&MainBoard_Online_Update_Status,&PowerBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
}
void SensorBoard_Online_Update_Callback(void)
{
    unsigned char data[sizeof(MainBoard_Online_Update)];
    memcpy(&MainBoard_Online_Update_Status,&SensorBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
}
void UltBoard_Online_Update_Callback(void)
{
    unsigned char data[sizeof(MainBoard_Online_Update)];
    memcpy(&MainBoard_Online_Update_Status,&UltBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
}
void LightBoard_Online_Update_Callback(void)
{
    unsigned char data[sizeof(MainBoard_Online_Update)];
    memcpy(&MainBoard_Online_Update_Status,&LightBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    memcpy(&data,&MainBoard_Online_Update_Status,sizeof(MainBoard_Online_Update));
    publishFrameData(TX2_ID,MainBoard_Online_Update_ID,data,sizeof(MainBoard_Online_Update));
}
