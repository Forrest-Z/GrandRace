#ifndef _LOG_APP_H_
#define _LOG_APP_H_

#include <stdint.h>
#include <stdbool.h>

#define LOG_CMD_NONE 		0 //默认
#define LOG_CMD_READ 		1 //读取日志
#define LOG_CMD_CLEAN 	2	//清除日志
#define LOG_CMD_FLUSH 	3	//刷新日志


void App_log_cmd(void);

void Log_syscall_cmd(const char* cmd);
void logCmdHanlder(void);
void joys_log_tp_SubcallBack(void);
void chassisReboot(void);
extern volatile bool g_bVersion;
uint8_t getSdFlashState(void);
void Log_version(void);


#endif

