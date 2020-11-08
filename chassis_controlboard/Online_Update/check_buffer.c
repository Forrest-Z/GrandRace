#include "check_buffer.h"
#include "usbd_cdc.h"
#define crc32_ok 1
#define crc32_error 0

extern USBD_HandleTypeDef hUsbDeviceFS;
typedef  void (*pFunction)(void);
pFunction JumpToApplication;
uint32_t JumpAddress;


void Jump_Appcation(void)
{
    if(((*(__IO uint32_t*)IAP_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
        __disable_irq();
        NVIC_SystemReset();
    }
}
uint8_t IAP_IsValidProgram( const IAP_HeadFrame *p_iap_frame )
{
    uint32_t crc32_result;
    if(p_iap_frame->version == 0x0001)
    {
        crc32_result = HAL_CRC_Calculate( &hcrc, (uint32_t*)&(p_iap_frame->program_data), (p_iap_frame->length-1024)/4);

        if( p_iap_frame->crc32 == crc32_result )
        {
            return crc32_ok;
        }
        else
        {
            return crc32_error;
        }
    }
    else
    {
        return crc32_error;
    }
}
void check_buffer(void)
{
    uint8_t program_valid_flag;
    if( IAP_IsValidProgram( (const IAP_HeadFrame *)APPLICATION_BUFFER_ADDRESS ) )
    {
        if( IAP_IsValidProgram( (const IAP_HeadFrame *)APPLICATION_ADDRESS ) )
        {

            if( ((const IAP_HeadFrame *)APPLICATION_ADDRESS)->crc32 != ((const IAP_HeadFrame *)APPLICATION_BUFFER_ADDRESS)->crc32 )
            {
                Send_Mcu_Buffer(Update_Ok);
                program_valid_flag = 1;
                HAL_Delay(100);
            }
            else
            {
                Send_Mcu_Buffer(Same_Volid);
                program_valid_flag = 0;
                HAL_Delay(100);
            }
        }
        else
        {
            Send_Mcu_Buffer(Update_Ok);
            program_valid_flag = 1;
            HAL_Delay(100);
        }
    }
    else
    {
        if( IAP_IsValidProgram( (const IAP_HeadFrame *)APPLICATION_ADDRESS ) )
        {
            Send_Mcu_Buffer(Update_Error);
            program_valid_flag = 0;
            HAL_Delay(100);
        }
        else
        {
            Send_Mcu_Buffer(No_Valid_Code);
            program_valid_flag = 0;
            HAL_Delay(100);
        }
    }

    if( program_valid_flag )
    {
        Jump_Appcation();
    }
}
