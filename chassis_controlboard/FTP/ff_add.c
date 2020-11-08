
#include "./ff_add.h"

int32_t Filenum;

char Listbuf[2048];

FRESULT FTP_scan_files(char* path, int * buf_len)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int len, buf_ptr = 0;
    char *fn; 	/* This function is assuming no_Unicode cfg.*/
    char date_str[15];

    Filenum = 0;

    res = f_opendir(&dir, path);
    if(res) return res;

    for(;;)
    {
        vTaskSuspend(CommunicationHandle);
//			Suspend_Task_All();
        res = f_readdir(&dir, &fno);
//			Resume_Task_All();
        vTaskResume(CommunicationHandle);
        if(res != FR_OK || fno.fname[0] == 0) break;
        if(fno.fname[0] == '.') continue;

        fn = fno.fname;

        /*add date*/
        len = sprintf(date_str, "JUL ");
        sprintf(date_str+len, "%d %d",16,2019);

        /*add fattrib*/
        if(fno.fattrib & AM_DIR)
        {
            sprintf(Listbuf+buf_ptr, "d");
        } else
        {
            sprintf(Listbuf+buf_ptr, "-");
        }
        buf_ptr++;

        len = sprintf(Listbuf + buf_ptr, "rwxr-xr-x 1 ftp ftp %d %s %s\r\n", (int)fno.fsize, date_str, fn);
        buf_ptr += len;

        Filenum ++;
        if(Filenum >= 35)   /*only read 30 files*/
        {
            break;
        }
    }

    return res;
}

#if 0
FRESULT scan_files(char* path, char *buf, int * buf_len)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int len, buf_ptr = 0;
    char *fn; 	/* This function is assuming no_Unicode cfg.*/
    char date_str[15];
    int date_str_ptr = 0;

    Filenum = 0;

    res = f_opendir(&dir, path);
    if(res) return res;

    for(;;)
    {
        res = f_readdir(&dir, &fno);
        if(res != FR_OK || fno.fname[0] == 0) break;
        if(fno.fname[0] == '.') continue;

        fn = fno.fname;

        /*add date*/

        switch((fno.fdate >> 5) & 0x0f)
        {
        case 1:
            len = sprintf(date_str, "JAN ");
            break;
        case 2:
            len = sprintf(date_str, "FEB ");
            break;
        case 3:
            len = sprintf(date_str, "MAR ");
            break;
        case 4:
            len = sprintf(date_str, "APR ");
            break;
        case 5:
            len = sprintf(date_str, "MAY ");
            break;
        case 6:
            len = sprintf(date_str, "JUN ");
            break;
        case 7:
            len = sprintf(date_str, "JUL ");
            break;
        case 8:
            len = sprintf(date_str, "AUG ");
            break;
        case 9:
            len = sprintf(date_str, "SEP ");
            break;
        case 10:
            len = sprintf(date_str, "OCT ");
            break;
        case 11:
            len = sprintf(date_str, "NOV ");
            break;
        case 12:
            len = sprintf(date_str, "DEC ");
            break;
        }
        date_str_ptr += len;
        len = sprintf(date_str + date_str_ptr, "%d ", (fno.fdate & 0x1f));
        date_str_ptr += len;
        len = sprintf(date_str + date_str_ptr, "%d", (((fno.fdate >> 9) & 0x7f) + 1980));
        date_str_ptr = 0;

        /*add fattrib*/
        if(fno.fattrib & AM_DIR)
        {
            sprintf(buf + buf_ptr, "d");
        } else
        {
            sprintf(buf + buf_ptr, "-");
        }
        buf_ptr++;

        len = sprintf(buf + buf_ptr, "rwxr-xr-x 1 ftp ftp %d %s %s\r\n", (int)fno.fsize, date_str, fn);
        buf_ptr += len;

        Filenum ++;

    }
    return res;
}
#endif

int get_filesize(char* path, char *filename)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
//	int i, len, buf_ptr = 0;
    char *fn; 	/* This function is assuming no_Unicode cfg.*/

    if(*path == 0x00)
        res = f_opendir(&dir, "/");
    else
        res = f_opendir(&dir, path);
    //printf("f_opendir res: %d\r\n", res);
    if(res == FR_OK)
    {
        for(;;)
        {
            res = f_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0) break;
            if(fno.fname[0] == '.') continue;

            fn = fno.fname;

            if(!strcmp(fn, filename))
            {
                if(fno.fattrib & AM_DIR) {
                    //printf("\r\n%s/%s is a directory\r\n", path, filename);
                    return 0;
                }
                return fno.fsize;
            }
        }
        //printf("\r\n%s/%s was not found\r\n", path, filename);
        //f_closedir(&dir);
    }
    return -1;
}

void Suspend_Task(void)
{
//	taskENTER_CRITICAL();
//	vTaskSuspend(CommunicationHandle);
    vTaskSuspend(ControlHandle);
    vTaskSuspend(SensorHandle);
//	vTaskSuspend(StateMachineHandle);
    vTaskSuspend(LOGTaskHandle);
//	taskEXIT_CRITICAL();
}

void Resume_Task(void)
{
//	taskENTER_CRITICAL();
//	vTaskResume(CommunicationHandle);
    vTaskResume(ControlHandle);
    vTaskResume(SensorHandle);
//	vTaskResume(StateMachineHandle);
    vTaskResume(LOGTaskHandle);
//	taskEXIT_CRITICAL();
}

void Suspend_Task_All(void)
{
    vTaskSuspend(CommunicationHandle);
    vTaskSuspend(ControlHandle);
    vTaskSuspend(SensorHandle);
    vTaskSuspend(StateMachineHandle);
    vTaskSuspend(LOGTaskHandle);
}

void Resume_Task_All(void)
{
    vTaskResume(CommunicationHandle);
    vTaskResume(ControlHandle);
    vTaskResume(SensorHandle);
    vTaskResume(StateMachineHandle);
    vTaskResume(LOGTaskHandle);
}
