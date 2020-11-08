#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__
#include "stm32f4xx_hal.h"
#include "buffer.h"
#include "info_core.h"
#include <string.h>
#include "Online_Downloader.h"
#pragma pack(1)

#define ready_error 0
#define ready_ok 1
#define recivce_ok 2
#define recivce_error 3
#define Update_Ok 4
#define Update_Error 5
#define Same_Volid 6
#define No_Valid_Code 7
#define last_recivce_ok 8
typedef struct {
    uint8_t ID_Code;
    uint8_t Function_Code;
} Receive_Control_Buffer;
typedef struct {
    uint8_t board_type;
    uint8_t Status;
    uint16_t num;
} MainBoard_Online_Update;
extern Receive_Control_Buffer Receive_407_Buffer;
extern MainBoard_Online_Update MainBoard_Online_Update_Status ;
extern MainBoard_Online_Update PowerBoard_Online_Update_Status;
extern MainBoard_Online_Update SensorBoard_Online_Update_Status;
extern MainBoard_Online_Update UltBoard_Online_Update_Status;
extern MainBoard_Online_Update LightBoard_Online_Update_Status;
void Init_Communication(void);
void Send_Mcu_Buffer(uint8_t buffer_type);
void down_send(uint8_t board_type);
#endif

