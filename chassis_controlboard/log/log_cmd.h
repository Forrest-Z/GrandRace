#ifndef _LOG_CMD_H_
#define _LOG_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#pragma pack(1)

typedef enum {
    eLog_help_id = 0,
    eLog_version_id,
    eLog_main_version_id,
    eLog_power_version_id,
    eLog_joy_version_id,
    eLog_ult_version_id,
    eLog_led_version_id,
    eLog_log_sd_id,
    eLog_log_set_level_id,
    eLog_log_get_level_id,
    eLog_set_env_id,
    eLog_get_env_id,
    eLog_del_env_id,
    eLog_set_env_f_id,
    eLog_get_env_f_id,
    eLog_reboot_id,
    eLog_get_para_id,
    eLog_maintain_id,
    eLog_joymod_id,
    eLog_reset_gyro_id,
    eLog_reset_joys_id,
    eLog_power_ctl_id,
    elog_led_ctl_id,
    eLog_set_ult_ch_id,
    eLog_get_ult_ch_id, 
		eLog_ftp_id,
		eLog_ult_reboot_id,
    eLog_tp_main_id,
    eLog_err,
} eLog_call;


typedef struct _log_syscall
{
    const eLog_call	id;		/* system call id */
    const char*		cmd;		/* system call cmd */
    const char*		name;		/* the name of system call */
    const char*		desc;		/* description of system call */
} log_syscall;

#define LOG_COMMAND_NUM	(sizeof(_syscall_table)/sizeof(log_syscall))
log_syscall _syscall_table[] =
{
    {eLog_help_id,				  "help", 			  "help         ", "- Show help information"},
    {eLog_version_id,				"version",      "version  		 ", "- Show main power joy system version information"},
    {eLog_main_version_id,	"main_version", "main_version  ", "- Show main system version information"},
    {eLog_power_version_id,	"power_version","power_version ", "- Show power system version information"},
    {eLog_joy_version_id,		"joy_version", 	"joy_version   ", "- Show joy system version information"},
    {eLog_ult_version_id,		"ult_version", 	"ult_version   ", "- Show ult system version information"},
    {eLog_led_version_id,		"led_version", 	"led_version   ", "- Show led system version information"},
    {eLog_log_sd_id,			  "log_sd", 			"log_sd       ", "- Master [<read>/<clean>/<flush>] sd log"},
    {eLog_log_set_level_id,	"set_log_level","set_log_level", "- Set[(4:dbg)|(3:info)|(2:warn)|(1:error)]for eg.[set_log_level 4] "},
    {eLog_log_get_level_id, "get_log_level","get_log_level", "- get[(4:dbg)|(3:info)|(2:warn)|(1:error)]for eg.[get_log_level] "},
    {eLog_set_env_id,			"set_env", 	  	"set_env      ", "- set_env para(motor driver chassis) int [key value]"},
    {eLog_get_env_id,			"get_env", 	  	"get_env      ", "- get_env para(motor driver chassis) int [key value]"},
    {eLog_del_env_id,			"del_env", 	  	"del_env      ", "- Delete environment parameter [key]"},
    {eLog_set_env_f_id,   "set_f_env",    "set_f_env    ", "- set_f_env para(joy_l joy_a reset_odom) float [key value]"},
    {eLog_get_env_f_id,   "get_f_env",    "get_f_env    ", "- get_f_env para(joy_l joy_a reset_odom) float [key value]"},
    {eLog_reboot_id,			"reboot", 	    "reboot       ", "- Restart chassis system"},
    {eLog_get_para_id,		"get_para", 	  "get_para     ", "- Get system parameter"},
    {eLog_maintain_id, 		"set_maintain", "set_maintain ", "- change maintenance mode  eg. set_maintain 0|1"},
    {eLog_joymod_id, 			"set_joymod",   "set_joymod ", "- change set_joys mode or normal mode eg. set_joymod 0|1"},
    {eLog_reset_gyro_id,	"reset_gyro",  	"reset_gyro   ", "- It will reset gyro of IMU eg. reset_gyro" },
    {eLog_reset_joys_id,  "reset_joy",    "reset_joy   ", "- It will reset joys  eg. reset_joy " },
    {eLog_power_ctl_id,   "power",        "power   ",  "- power [tx2/break/pad/12v/all/ult ] [0/1],eg. power tx2 1" },
    {elog_led_ctl_id,   	"led",        	"led   ",    "- led [cmd], eg. led 3 500" },
    {eLog_set_ult_ch_id,  "set_ult_ch",   "set_ult_ch ",  "- set_ult_ch [ch] [cmd] eg. set_ult_ch 3 0|1 " },
    {eLog_get_ult_ch_id,  "get_ult_ch",   "get_ult_ch ",  "- get_ult_ch [ch] [cmd] eg. get_ult_ch 3"},
    {eLog_ftp_id,         "ftp",          "tfp   ",    "- ftp $cmd: creat | del,eg. ftp creat"},
    {eLog_ult_reboot_id,  "ult_reboot",   "ult_reboot",    "- ult_reboot  | del,eg. ult_reboot"},
    {eLog_tp_main_id, 		"tp_main", 			"tp_main  ", "- tp_main [set_tp get_tp get_list_tp] $id,$en [tp_main set_tp 3 1]"},
};

typedef struct
{
    uint8_t state;
    uint8_t cmd;
    uint16_t date_len;
    uint8_t data[128];
} stLog_sd;


#endif

