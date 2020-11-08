#ifndef __SD_LOG_APP_H
#define __SD_LOG_APP_H
#include "SD_Log_Driver.h"
#include <string.h>
#include <stdarg.h>
#include "chassis_config.h"

#define EF_RW_ERROR       	0 //没有该log文件 读取失败
#define EF_RW_OK       		  1 //读取成功
#define EF_SYSTEM_BUSY      3 //系统忙

#define FATFS_INITOK 1
#define FATFS_INITERROR 0


#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 1




void file_manage_app(void);
uint8_t get_fatfs_initstatus_app(void);
uint8_t Dele_envinfo_app(char* key_word);
void sdlog_printf_app(const char *format, ...);

void sd_printf(const char *format, ...);
char write_log_app(char* buffer);

uint8_t read_log_app(char *file_name);
uint8_t read_sd_app(void);
uint8_t delect_file_app(char* file_name);
uint8_t write_env_app(char* key_word,char *vaule);
char* read_env_app(char* key_word);
void SDLog_info_push(void);
void SDLog_info_port(const char *format, ...) ;

//设置日志等级
void setLogLevel(uint8_t _level);
//获取日志等级
uint8_t getLogLevel(void);

/*================================================================
* 日志对外接口
=================================================================*/
//sd_printf("DEBUG_[%s-%s|%d]:"msg, pcTaskGetTaskName(osThreadGetId()), __FUNCTION__, __LINE__, ## arg);

#define GR_LOG_DEBUG(msg, arg...) \
	if (getLogLevel() == LOG_LEVEL_DEBUG ) { \
		sd_printf("|%d|DEBUG|[%s|%d]:"msg,osThreadGetId(), __FUNCTION__, __LINE__, ## arg); \
	}

#define GR_LOG_INFO(msg, arg...) \
	if (getLogLevel() >= LOG_LEVEL_INFO) { \
		sd_printf("|%d|INFO|[%s|%d]:"msg,osThreadGetId(), __FUNCTION__, __LINE__, ## arg); \
	}

#define GR_LOG_WARN(msg, arg...) \
	if (getLogLevel() >= LOG_LEVEL_WARN) { \
		sd_printf("|%d|WARN|[%s|%d]:"msg,osThreadGetId(), __FUNCTION__, __LINE__, ## arg); \
	}

#define GR_LOG_ERROR(msg, arg...) \
	if (getLogLevel() >= LOG_LEVEL_ERROR) { \
		sd_printf("|%d|ERROR|[%s|%d]:"msg,osThreadGetId(), __FUNCTION__, __LINE__, ## arg); \
	}


#define GR_LOG_LINE(msg, arg...) \
    if (getLogLevel() >= LOG_LEVEL_ERROR) { \
      sd_printf(msg, ## arg); \
    }


#endif
