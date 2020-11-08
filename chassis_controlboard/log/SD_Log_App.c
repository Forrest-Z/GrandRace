#include "SD_Log_App.h"


static uint8_t sd_log_level = LOG_LEVEL_DEBUG;

//设置日志等级
void setLogLevel(uint8_t _level)
{
    sd_log_level = _level;
}
//获取日志等级
uint8_t getLogLevel(void)
{
    return sd_log_level;
}


static char log_info_buf[LOG_INFO_MAX_LEN] = {0};
static uint16_t log_info_cnt = 0;
void file_manage_app(void)
{
    if(check_SD_exit())
    {
        file_manage();
    }
}
uint8_t get_fatfs_initstatus_app(void)
{
    if((FAT_LIKINT == 0)&&(FAT_FMOINT == 0))
    {
        return FATFS_INITOK;
    }
    else
    {
        return FATFS_INITERROR;
    }
}

char write_log_app(char* buffer)
{
    if(get_fatfs_initstatus_app())
    {
        if (Write_LOGinfo(buffer) == LOG_RW_ERROR)
          return Write_LOGinfo(buffer);
    }
    else
    {
        return LOG_RW_ERROR;
    }
		return LOG_RW_ERROR;
}

uint8_t read_log_app(char *file_name)
{
    if(get_fatfs_initstatus_app())
    {
        return Read_LOGinfo(file_name);
    }
    else
    {
        return LOG_RW_ERROR;
    }
}
uint8_t read_sd_app(void)
{
    return Read_SDInfo();
}
uint8_t delect_file_app(char* file_name)
{
    return Declet_File(file_name);
}
uint8_t write_env_app(char* key_word,char *vaule)
{
    if(get_fatfs_initstatus_app())
    {
        return Write_envinfo(key_word,vaule);
    }
    else
    {
        return LOG_RW_ERROR;
    }
}
char* read_env_app(char* key_word)
{
    char *str_para = NULL;
    for(unsigned char i=0; i<3; i++)
    {
        str_para = Read_envinfo(key_word);
        if(str_para != NULL)
        {
            return str_para;
        }
        osDelay(10);
    }
    return NULL;
}

void sdlog_printf_app(const char *format, ...)
{
    uint16_t len;
    va_list args;
    static char buf[256] = {0};

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */

    len = vsprintf(buf, format, args);
    if(len > 256)
        len = 256;

    if(len > 0)
    {
        sdlog_info_send(buf, len);
    }

    va_end(args);
}
uint8_t Dele_envinfo_app(char* key_word)
{
    return Dele_envinfo(key_word);
}
/**********write log*****************/
void sd_printf(const char *format, ...)
{
    uint16_t len;
    va_list args;
    static char buf[256] = {0};

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */

    len = vsprintf(buf, format, args);
    if(len > 256)
        len = 256;

    if(len > 0)
    {
        write_log_app(buf);
    }

    va_end(args);
}

void SDLog_info_push(void)
{
    if(log_info_cnt > 0)
    {
        if(log_info_cnt > 256)
            log_info_cnt = 256;

        sdlog_info_send(log_info_buf, log_info_cnt);

        memset(log_info_buf, 0x00, log_info_cnt);
        log_info_cnt = 0;
    }
}

void SDLog_info_port(const char *format, ...)
{
    uint8_t len;
    va_list args;
    static char buf[256] = {0};

    /* args point to the first variable parameter */
    va_start(args, format);
    /* must use vprintf to print */

    len = vsprintf(buf, format, args);
    if(len > 0)
    {
        if(len + log_info_cnt >= LOG_INFO_MAX_LEN)
        {
            SDLog_info_push();
        }
        memcpy(&log_info_buf[log_info_cnt], buf, len);
        log_info_cnt += len;
    }

    va_end(args);
}
