#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include "SD_Driver.h"
#include "info_core.h"
#include "Clock.h"
#include "Publish_Pc.h"
#include "freertos.h"
#include <stdbool.h>
#include "watchdog_task.h"


#define ENV_SIZI 							30
#define LOG_INFO_MAX_LEN 			256

#define LOG_RW_ERROR       	0 //没有该log文件 读取失败
#define LOG_RW_OK       		1 //读取成功
#define LOG_RW_END_ERR      2 //未读取完毕
#define LOG_SYSTEM_BUSY     3 //系统忙

//日志名称
typedef enum
{
    eFILE_TEMP    = 0, 	//TEMP_LOG
    eFILE_SD_INFO = 1, //eFILE_SD_INFO
    eFILE_ENV     = 2,  //env_file
    eFILE_LOG     = 3,  //log_file
} eLOG_NAME;


typedef struct {
    char LOG_DAY[20];
    char LOG_TIME[20];
} LOGTIME;

typedef struct {
    uint8_t TEMP_FILE_EXIST;
    uint8_t LOG_FILE_EXIST;
} LOGFILESTATUS;

//最大log数目
#define LOG_NUM_MAX 	10
typedef struct {
    //注意最长日志长度不能超过200个字节
    char buffer_info[LOG_NUM_MAX][200];
    uint8_t num;
} SAVE_BUFFER;
typedef struct {
     char Year;
     char month;
		 char day;
} LOGFILETIME;
extern LOGTIME logtime;


void getlogday(void);
void sdlog_info_send(char* info, size_t len);
void creat_file_sdInfo(void);
uint8_t file_manage(void);
uint8_t creat_file_name(eLOG_NAME name);
uint8_t Declet_File(char *file_name);
uint8_t Copy_File(char *from_Name, char *to_Name);
uint8_t Write_LOGinfo(char *buffer);
uint8_t sync_log(void);
uint8_t Write_envinfo(char* key_word,char *vaule);
uint8_t Read_LOGinfo(char *file_name);


uint8_t Read_SDInfo(void);
char* Read_envinfo(char* key_word);
uint8_t Dele_envinfo(char* key_word);
FRESULT scan_files (void);
void Log_Init(void);
extern osMutexId RAM_BUSYHandle;
extern osMutexId SD_BUSYHandle;

#if 0
uint32_t getFileContent(
    char * fileName,
    char * buf,
    uint32_t startPos,
    uint16_t readSize,
    uint32_t * curPos,
    bool * readFinishFlag);
uint8_t readLogInfo(char * fileName);

#endif

#endif

