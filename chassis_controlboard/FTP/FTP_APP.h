#ifndef _FTP_APP_H
#define _FTP_APP_H

#include "ftpd.h"
#include "w5500_conf.h"
#include "chassis_fsm.h"
#include "cmsis_os.h"

enum ftp_task_state
{
    ftp_task_none = 0,
    ftp_task_ok
};

enum ftp_creat_state
{
    ftp_creat_0 = 0,  /*none*/
    ftp_creat_1,			/*ftp read state*/
    ftp_creat_2,			/*ftp connect state*/
    ftp_creat_3,			/*ftp creat overtime*/
    ftp_creat_4,			/*ftp state machine error*/
    ftp_creat_5				/*ftp close state*/
};

enum ftp_del_state
{
    ftp_del_0 = 0,		/*none*/
    ftp_del_1,				/*ftp del ok*/
    ftp_del_2,				/*ftp del error*/
};


extern void ftp_Task(void const * argument);
extern void ftp_CreatTask(void);
extern void ftp_Del_Task(void);

#endif



