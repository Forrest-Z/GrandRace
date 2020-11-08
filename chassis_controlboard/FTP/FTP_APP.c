
#include "FTP_APP.h"
#include "ff_add.h"

uint8_t gFTPBUF[1024];
uint8_t ftp_Task_state = 0;
uint32_t FTP_CNT=0;

osThreadId ftpHandle;
extern osSemaphoreId creatfullHandle;

/**********************ftp task******************************/
void ftp_Task(void const * argument)
{
    ftpd_init(ConfigMsg.lip);
    for(;;)
    {
        test_task = 6;
        ftpd_run(gFTPBUF);
        if(FTP_LISTEN == FTP_Status)
        {
            osSemaphoreRelease(creatfullHandle);
        }
        if(FTP_TimeOut == 0xFF)
        {
            ftp_Task_state = ftp_task_none;
            Resume_Task();
            vTaskDelete(ftpHandle);
        }
        Watchdog_Task_Feed(TASK_BIT_0 | TASK_BIT_1 | TASK_BIT_4);
        osDelay(100);
    }
}

/*********************ftp publish**********************************/

void ftp_creat_publish(uint8_t Data)
{
    char buffer[20];
    uint8_t i;
    for(i=0; i<sizeof(buffer); i++)
    {
        buffer[i] = 0;
    }
    sprintf(buffer,"ftp creat %d",Data);
//	publishFrameData(TX2_ID,LOG_UP_ID, (unsigned char *)buffer, strlen(buffer));
    sdlog_printf_app("%s",buffer);
}

void ftp_del_publish(uint8_t Data)
{
    char buffer[20];
    uint8_t i;
    for(i=0; i<sizeof(buffer); i++)
    {
        buffer[i] = 0;
    }
    sprintf(buffer,"ftp del %d",Data);
    //publishFrameData(TX2_ID,LOG_UP_ID, (unsigned char *)buffer, strlen(buffer));
    sdlog_printf_app("%s",buffer);
}

/**********************creat ftp task******************************/
static void creat_ftp_task()
{
    uint8_t i = 5;
//	taskENTER_CRITICAL();
    osThreadDef(ftp, ftp_Task, osPriorityLow, 0, 1024);
    do
    {
        ftpHandle = osThreadCreate(osThread(ftp), NULL);
        if(ftpHandle != NULL) break;
        i--;
    }	while(i);
//	taskEXIT_CRITICAL();
    if(i == 0)
    {
        ftp_Task_state = ftp_task_none;
    }
    else
    {
        ftp_Task_state = ftp_task_ok;
        GR_LOG_INFO("==FTP task create ok ==");
        Suspend_Task();
        FTP_CNT++;
    }
}

static uint8_t check_ftp_status()
{
    int32_t flag;
    flag = osSemaphoreWait(creatfullHandle,1000);
    if(flag == osOK) return ftp_creat_1;
    else return ftp_creat_3;
}

void ftp_CreatTask()
{
    uint8_t flag = ftp_creat_0;

    /*reserve state machine*/

    if(ftp_Task_state == ftp_task_none)
    {
        ftp_Task_state = ftp_task_ok;
        creat_ftp_task();
        flag = check_ftp_status();

    }
    else
    {
        switch(FTP_Status)
        {
        case 	FTP_CLOSED:
            vTaskDelete(ftpHandle);
            creat_ftp_task();
            flag = check_ftp_status();
            break;
        case FTP_INIT:    //ready
            flag = ftp_creat_1;
            break;
        case FTP_ESTABLISHED:  //connet
            flag = ftp_creat_2;
            break;
        default:
            break;
        }
    }
    ftp_creat_publish(flag);
}

/*******************************del ftp Task****************************/
static void Del_ftp(void)
{
    del_flag = 0xFF;
    osSemaphoreWait(creatfullHandle,osWaitForever);
    osSemaphoreWait(creatfullHandle,osWaitForever);
}

void ftp_Del_Task()
{
    uint8_t flag;
    if(ftp_Task_state == ftp_task_ok)
    {
        Del_ftp();
        vTaskDelete(ftpHandle);
        ftp_Task_state = ftp_task_none;
        del_flag = 0;
        Resume_Task();
    }

    flag = ftp_del_1;
    GR_LOG_INFO("==FTP task delete ok ==");
    ftp_del_publish(flag);
}
