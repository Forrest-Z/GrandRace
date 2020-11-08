#ifndef __ONLINE_DOWNLOADER_H__
#define __ONLINE_DOWNLOADER_H__
#include "stm32f4xx_hal.h"
#include "Communication.h"
#include "flash_if.h"
#include "check_buffer.h"
#define init_flash 1
#define send_begin 2
#define send_end   3
#define write_length 128
void Online_Update_Callback(void);
void PowerBoard_Online_Update_Callback(void);
void SensorBoard_Online_Update_Callback(void);
void UltBoard_Online_Update_Callback(void);
void LightBoard_Online_Update_Callback(void);
extern uint8_t Start_Get_Bin ;
extern uint8_t Start_Check_Bin ;
#endif
