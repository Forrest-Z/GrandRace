#ifndef __SD_DRIVER_H
#define __SD_DRIVER_H

#include "fatfs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "SD_Log_App.h"
#pragma pack(1)

#define SD_SPACE_FREE 1
#define SD_SPACE_FULL 0

#define SD_EXIT       0 //拔出
#define SD_INSET      1 //插入

#define SD_STATE_OK 								0  //正常
#define SD_STATE_EXIT_BIT 					0  //未插入 位
#define SD_STATE_FULL_BIT  					1  //SD卡满 位

typedef struct
{
    float total_size;  //units - KB
    float free_size;		//units - KB
    uint8_t sd_exit;			//if 1£¬ sd unlink, or linking
} sd_info_t;


extern sd_info_t SD_INFO;

uint8_t check_SD_exit(void);
void SD_App_Init(void);
uint8_t Get_Sdtorage_spacestatus(void);
float Get_SD_Size(sd_info_t *sd);
#endif
