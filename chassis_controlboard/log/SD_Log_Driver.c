#include "SD_Log_Driver.h"

uint32_t byte_w;
uint32_t byte_r;
LOGTIME logtime;

LOGFILESTATUS logfilestatus;
SAVE_BUFFER save_buffer;
extern osThreadId CommunicationHandle;
extern osThreadId SensorHandle;
extern osThreadId ControlHandle;
extern osThreadId StateMachineHandle;
char logsystime[20];

const char logName[3][15] = {
    "TEMP_LOG.txt",
    "SD_info.txt",
    "env_file.txt",
};

/* 互斥信号量句柄 */
SemaphoreHandle_t xSemaphore = NULL;

void sdlog_info_send(char* info, size_t len)
{
    if(len > LOG_INFO_MAX_LEN)
        len = LOG_INFO_MAX_LEN;

    if(len > 128)
    {
        LogUpPubHandle(info, 128);
        LogUpPubHandle(&info[128], len - 128);
    }
    else
        LogUpPubHandle(info, len);
}

void getlogday(void)
{
    RTime_TypeDef time;
    getSystemTime(&time);
    sprintf(logtime.LOG_DAY,"20%02d-%02d-%02d.txt",time.year,time.mon,time.day);
}
void getlogtime(void)
{
    RTime_TypeDef time;
    getSystemTime(&time);
    sprintf(logtime.LOG_TIME,"%02d:%02d:%02d ",time.hour,time.min,time.sec);
}
void getsystime(void)
{
    uint32_t systime;

    systime =readSystemClock();
    sprintf(logsystime, "%05d ",systime);
}

uint8_t file_manage(void)
{
    char creatlogtime[20];

    memcpy(creatlogtime,logtime.LOG_DAY,sizeof(logtime.LOG_DAY));
    getlogday();

    if((getTX2CommunicationState())&&(strncmp(creatlogtime,
            logtime.LOG_DAY, strlen(logtime.LOG_DAY)) != 0))
    {
        creat_file_name(eFILE_LOG);
        logfilestatus.LOG_FILE_EXIST = 1;
    }
    if((getTX2CommunicationState())&&(logfilestatus.TEMP_FILE_EXIST))
    {
        creat_file_name(eFILE_LOG);
        logfilestatus.LOG_FILE_EXIST = 1;

        Copy_File("TEMP_LOG.txt",logtime.LOG_DAY);
        logfilestatus.TEMP_FILE_EXIST = 0;
    }
    if((getTX2CommunicationState())&&(!logfilestatus.LOG_FILE_EXIST))
    {
        creat_file_name(eFILE_LOG);
        logfilestatus.LOG_FILE_EXIST = 1;
    }
    return 1;
}

/*************************************************
Function: 			creat_file_name
Description:		新建文件名
Input:	name:要创建文件名类型
Return: 				是否成功
*************************************************/
uint8_t creat_file_name(eLOG_NAME name)
{
    FIL fil;
    uint8_t ret;
    char file_name[20] = {0};

    osMutexWait(SD_BUSYHandle,osWaitForever);
    if(name == eFILE_LOG)
    {
        //只有日志文件才复制
        getlogday();
        strncpy(file_name,logtime.LOG_DAY,sizeof(file_name));
    }
    else
    {
      strncpy(file_name,logName[name],sizeof(file_name));
    }

    //打开文件。如果文件不存在，则打开失败。(默认)
    ret = f_open(&fil,file_name,FA_OPEN_EXISTING);
    if(ret != FR_OK)
    {
        //创建一个新文件。如果文件已存在，则创建失败
        ret = f_open(&fil, file_name, FA_CREATE_NEW|FA_WRITE);
    }

    //如果是SD 信息 容量文件
    if(name == eFILE_SD_INFO && ret == FR_OK)
    {
      ret = f_write(&fil,&SD_INFO,sizeof(sd_info_t),&byte_w);
    }
        
    f_close(&fil);
    osMutexRelease(SD_BUSYHandle);
    return ret;
}
/*************************************************
Function: 			log_file_status_init
Description:		初始化文件状态
Input:	none
Return: none
*************************************************/
void log_file_status_init(void)
{
    if(getTX2CommunicationState())
    {
        //与主板通信成功
        logfilestatus.LOG_FILE_EXIST = 1;  //log日志使能
        logfilestatus.TEMP_FILE_EXIST = 0; //临时日志使能
    }
    else
    {
        //与主板通信失败
        logfilestatus.LOG_FILE_EXIST = 0;   //log日志使能
        logfilestatus.TEMP_FILE_EXIST = 1;  //临时日志使能
    }
}
uint8_t Declet_File(char *file_name)
{
    uint8_t ret;
    osMutexWait(SD_BUSYHandle,osWaitForever);

    ret = f_unlink(file_name);
    if(ret == FR_OK)
    {
        if (strncmp(file_name,logtime.LOG_DAY,strlen(logtime.LOG_DAY)) == 0)
        {
            logfilestatus.LOG_FILE_EXIST = 0;
        }
        if (strncmp(file_name, "TEMP_LOG.txt", 12) == 0)
        {
            logfilestatus.TEMP_FILE_EXIST = 0;
        }
        osMutexRelease(SD_BUSYHandle);
        return 1;									//delect ok
    }
    else
    {
        osMutexRelease(SD_BUSYHandle);
        return 0;								//delect error
    }
}
uint8_t Copy_File(char *from_Name, char *to_Name)
{
    FIL from_fl,to_fl;
    uint8_t ret;
    uint32_t from_size,to_size,free_size;
    char buf[1024];
    osMutexWait(SD_BUSYHandle,osWaitForever);

    ret = f_open(&from_fl, from_Name, FA_READ);
    if(ret != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        return 0;
    }

    ret = f_open(&to_fl, to_Name, FA_WRITE);
    if(ret != FR_OK)
    {
        f_close(&from_fl);
        osMutexRelease(SD_BUSYHandle);
        return 0;
    }

    from_size = f_size(&from_fl);
    to_size = f_size(&to_fl);

    free_size = from_size;
    while(free_size)
    {
        if(free_size > sizeof(buf))
        {
            f_read(&from_fl,buf,sizeof(buf),&byte_r);
            f_lseek(&to_fl,to_size+from_size-free_size);
            f_write(&to_fl,buf,sizeof(buf),&byte_w);
            f_sync(&to_fl);

            free_size -= sizeof(buf);
            f_lseek(&from_fl,from_size-free_size);
        }
        else
        {
            f_read(&from_fl,buf,free_size,&byte_r);
            f_lseek(&to_fl,to_size+from_size-free_size);
            f_write(&to_fl,buf,free_size,&byte_w);
            f_sync(&to_fl);
            free_size = 0;
        }
    }
    f_close(&from_fl);
    f_close(&to_fl);

    osMutexRelease(SD_BUSYHandle);

    Declet_File(from_Name);
    Declet_File("00-00-00.txt");
    


    return 0;
}

/*************************************************
Function: 			creat_file_sdInfo
Description:		新建SD信息文件
Input:	        void
Return: 				none
*************************************************/
void creat_file_sdInfo(void)
{
    FIL fil;
    uint8_t ret = FR_OK;
    osMutexWait(SD_BUSYHandle,osWaitForever);

    ret = f_open(&fil,"SD_info.txt",FA_OPEN_EXISTING);
    if(ret == FR_OK)
    {
        ret = f_close(&fil);
        ret = f_open(&fil, "SD_info.txt", FA_WRITE);
    }
    else
    {
        ret = f_open(&fil, "SD_info.txt", FA_CREATE_NEW|FA_WRITE);
    }

    if(ret == FR_OK)
    {
        ret = f_write(&fil,&SD_INFO,sizeof(sd_info_t),&byte_w);
    }
    ret = f_close(&fil);

    osMutexRelease(SD_BUSYHandle);
    return ;
    
}
/*get time ms*/
static void gettimems(void)
{
    uint32_t systime;

	systime =readSystemClock();
	sprintf(logsystime, "[%05d] ",systime);
}

uint8_t Write_LOGinfo(char *buffer)
{
    osMutexWait(RAM_BUSYHandle,osWaitForever);
    if(save_buffer.num < LOG_NUM_MAX)
    {
        if(logfilestatus.LOG_FILE_EXIST)
        {
            getlogtime();
            strcpy(&save_buffer.buffer_info[save_buffer.num][0],logtime.LOG_TIME);
			gettimems();
			strcat(&save_buffer.buffer_info[save_buffer.num][0],logsystime);
            strcat(&save_buffer.buffer_info[save_buffer.num][0],buffer);
            strcat(&save_buffer.buffer_info[save_buffer.num][0],"\r\n");
            save_buffer.num++;
        }
        else
        {
            getsystime();
            strcpy(&save_buffer.buffer_info[save_buffer.num][0],logsystime);
            strcat(&save_buffer.buffer_info[save_buffer.num][0],buffer);
            strcat(&save_buffer.buffer_info[save_buffer.num][0],"\r\n");
            save_buffer.num++;
        }
        osMutexRelease(RAM_BUSYHandle);
        return LOG_RW_OK;
    }
    else
    {   
        osMutexRelease(RAM_BUSYHandle);
        //如果 save_buffer.num = 10 呢？是不是需要 写：sync_log
        sync_log();
        return LOG_RW_ERROR;
    }
}
uint8_t sync_log(void)
{
    FIL fil;
    uint32_t size;
    uint8_t ret;
    if(save_buffer.num == 0)
    {
        return 0;
    }

    osMutexWait(SD_BUSYHandle,osWaitForever);

    if(logfilestatus.LOG_FILE_EXIST)
    {
        ret = f_open(&fil, logtime.LOG_DAY, FA_WRITE);
        if(ret != FR_OK)
        {
            f_close(&fil);
            osMutexRelease(SD_BUSYHandle);
            return 0;
        }
    }
    else
    {
        ret = f_open(&fil, "TEMP_LOG.txt", FA_WRITE);
        if(ret != FR_OK)
        {
            f_close(&fil);
            osMutexRelease(SD_BUSYHandle);
            return 0;
        }
    }

    size = f_size(&fil);
    ret = f_lseek(&fil,size);
    if(ret != FR_OK)
    {
        f_close(&fil);
        osMutexRelease(SD_BUSYHandle);
        return 0;
    }
    osMutexWait(RAM_BUSYHandle,osWaitForever);
    for(int i = 0; i<save_buffer.num; i++)
    {
        ret = f_write(&fil,&save_buffer.buffer_info[i][0],
						strlen(&save_buffer.buffer_info[i][0]),&byte_w);
        if(ret != FR_OK)
        {
            f_close(&fil);
            osMutexRelease(RAM_BUSYHandle);
            osMutexRelease(SD_BUSYHandle);
            return 0;
        }
    }
    ret = f_close(&fil);
    if(ret != FR_OK)
    {
        save_buffer.num = 0;
        osMutexRelease(RAM_BUSYHandle);
        osMutexRelease(SD_BUSYHandle);
        f_close(&fil);
        return 0;
    }
    save_buffer.num = 0;
    osMutexRelease(RAM_BUSYHandle);
    osMutexRelease(SD_BUSYHandle);
    return 1;

}
/*************************************************
Function: 			env_file_init
Description:		写入环境变量
Input:key_word:环境变量名称 vaule :要写的数据字符串
Return: 				是否成功
*************************************************/
#define  KEY_ENV_NUM 8
const char envKey[KEY_ENV_NUM][15] = {
    "total_mileage",
    "imu_offset",
    "reset_odom",
    "motor",
    "driver",
    "chassis",
    "joy_l",
    "joy_a",
};

void env_file_init(void)
{
    FIL fil;
    uint8_t i = 0;
    char buffer_env = 0;

    osMutexWait(SD_BUSYHandle,osWaitForever);
    osMutexWait(RAM_BUSYHandle,osWaitForever);

    if(f_open(&fil, logName[eFILE_ENV], FA_WRITE) != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        osMutexRelease(RAM_BUSYHandle);
        f_close(&fil);
        return ;
    }

    for(i=0; i<KEY_ENV_NUM; i++)
    {
        //memset(buffer_env,i,sizeof(buffer_env));
        //Return   =   CR  =   13   =   '\x0d'
				//NewLine  =   LF  =   10   =   '\x0a'
        buffer_env = 10;
        f_lseek(&fil,i*ENV_SIZI);
        f_write(&fil,(char*)&buffer_env,1,&byte_w);
    }

    f_close(&fil);
    osMutexRelease(SD_BUSYHandle);
    osMutexRelease(RAM_BUSYHandle);
    return;
}
/*************************************************
Function: 			Write_envinfo
Description:		写入环境变量
Input:key_word:环境变量名称 vaule :要写的数据字符串
Return: 				是否成功
*************************************************/
uint8_t Write_envinfo(char* key_word,char* vaule)
{
    FIL fil;
    uint8_t i = 0,ret = LOG_RW_OK;
    char buffer_env[ENV_SIZI] = {0};

    osMutexWait(SD_BUSYHandle,osWaitForever);
    osMutexWait(RAM_BUSYHandle,osWaitForever);

    if(f_open(&fil, logName[eFILE_ENV], FA_WRITE) != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        osMutexRelease(RAM_BUSYHandle);
        f_close(&fil);
        return LOG_RW_ERROR;
    }

    for(i=0; i<KEY_ENV_NUM; i++)
    {
        if (strncmp((char *)&envKey[i], key_word, strlen(envKey[i])) == 0)
        {
            memset(buffer_env,i,sizeof(buffer_env));
            strcpy(buffer_env,key_word);
            strcat(buffer_env," ");
            strcat(buffer_env,vaule);
					//Return   =   CR  =   13   =   '\x0d'
					//NewLine  =   LF  =   10   =   '\x0a'
            buffer_env[ENV_SIZI-1] = 10;
            f_lseek(&fil,i*ENV_SIZI);
            if ((f_write(&fil,buffer_env,ENV_SIZI,&byte_w))!= FR_OK)
                ret=LOG_RW_ERROR;

            f_close(&fil);
            osMutexRelease(SD_BUSYHandle);
            osMutexRelease(RAM_BUSYHandle);
            return ret;
        }
    }

    ret = LOG_RW_ERROR;
    f_close(&fil);
    osMutexRelease(SD_BUSYHandle);
    osMutexRelease(RAM_BUSYHandle);
    return ret;
}
/*************************************************
Function: 			Read_envinfo
Description:		读取环境变量
Input:key_word:环境变量名称
Return: vaule :要写的数据字符串
*************************************************/
char* Read_envinfo(char* key_word)
{
    FIL fil;
    char *vaule = NULL;
    uint8_t i = 0,ret = FR_OK;
    char buffer_info[ENV_SIZI] = {0};

    osMutexWait(SD_BUSYHandle,osWaitForever);
    ret = f_open(&fil, "env_file.txt", FA_READ);
    if(ret != FR_OK)
    {
        vaule = NULL;
    }

    for(i=0; i<KEY_ENV_NUM; i++)
    {
        if (strncmp((char *)&envKey[i], key_word, strlen(envKey[i])) == 0)
        {
            f_lseek(&fil,i*ENV_SIZI);
            f_read(&fil,buffer_info,ENV_SIZI,&byte_w);
            if(ret != FR_OK)
            {
                vaule = NULL;
            }
            vaule = &buffer_info[strlen(envKey[i])+1];

            f_close(&fil);
            if(*vaule == 0)
            {
                vaule = NULL;
            }

            osMutexRelease(SD_BUSYHandle);
            return vaule;
        }
    }

    f_close(&fil);
    vaule = NULL;
    osMutexRelease(SD_BUSYHandle);
    return vaule;
}
/*************************************************
Function: 			Dele_envinfo
Description:		删除环境变量
Input:key_word:环境变量名称
Return: vaule :要删除的数据字符串
*************************************************/
uint8_t Dele_envinfo(char* key_word)
{
    FIL fil;
    uint8_t i=0,ret = FR_OK;
    char buffer_info[ENV_SIZI] = {0};

    osMutexWait(SD_BUSYHandle,osWaitForever);

    ret = f_open(&fil, "env_file.txt", FA_WRITE);
    if(ret != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        return LOG_RW_ERROR;
    }

    for(i=0; i<KEY_ENV_NUM; i++)
    {
        if (strncmp((char *)&envKey[i], key_word, strlen(envKey[i])) == 0)
        {
            ret = f_lseek(&fil,i*ENV_SIZI);
            if(ret != FR_OK)
            {
                //f_close(&fil);
                osMutexRelease(SD_BUSYHandle);
                return LOG_RW_ERROR;
            }

            ret = f_write(&fil,buffer_info,ENV_SIZI,&byte_w);
            if(ret != FR_OK)
            {
                osMutexRelease(SD_BUSYHandle);
                return LOG_RW_ERROR;
            }
            ret = f_close(&fil);
            if(ret != FR_OK)
            {
                osMutexRelease(SD_BUSYHandle);
                return LOG_RW_ERROR;
            }
            osMutexRelease(SD_BUSYHandle);
            return LOG_RW_OK;
        }
    }

    f_close(&fil);
    osMutexRelease(SD_BUSYHandle);
    return LOG_RW_ERROR;
}

/*************************************************
Function: 			Read_LOGinfo
Description:		读取LOG日志信息
Input:file_name:log文件名
Return: vaule :要删除的数据字符串
*************************************************/
uint8_t Read_LOGinfo(char *file_name)
{
    FIL fil_read;
    static uint32_t size_read,lseek_num;
    static uint32_t read_log_num = 0;
    uint32_t read_size;
    uint8_t temp_read_num = 0;
    char loginfo_read_buffer[200];
    static TickType_t xLastReadTime = 0;

    osMutexWait(SD_BUSYHandle,osWaitForever);

    //先喂狗
    Watchdog_SectorErase_Reconfigure();

    retSD = f_open(&fil_read, file_name, FA_READ);
    if(retSD != FR_OK)
    {
        Watchdog_Task_Start();
        osMutexRelease(SD_BUSYHandle);
        return LOG_RW_ERROR;
    }

    if(read_log_num == 0)
    {
        size_read = f_size(&fil_read);
    }
    else
    {
        f_lseek(&fil_read,lseek_num);
    }

    read_size = 0;
    while((temp_read_num < LOG_NUM_MAX)&&(size_read != 0))
    {
        retSD = f_read(&fil_read,&loginfo_read_buffer[read_size],1,&byte_r);
        if(retSD == FR_OK)
        {
            if(loginfo_read_buffer[read_size] == '\r')
            {
                if(loginfo_read_buffer[0] == '\n')
                {
                    sdlog_info_send(&loginfo_read_buffer[1],read_size - 1);
                    read_log_num++;
                    temp_read_num++;
                }
                else
                {
                    sdlog_info_send(loginfo_read_buffer,read_size);
                    read_log_num++;
                    temp_read_num++;
                }
                read_size = 0;
                memset(loginfo_read_buffer,0,sizeof(loginfo_read_buffer));
            }
            else
            {
                read_size += byte_r;
            }
            size_read -= byte_r;
            lseek_num += byte_r;
        }
        else
        {
            osMutexRelease(SD_BUSYHandle);
            Watchdog_Task_Start();
            return LOG_RW_ERROR;
        }

        vTaskDelayUntil(&xLastReadTime, 10);
    }

    f_close(&fil_read);

    if(size_read != 0)
    {
        osMutexRelease(SD_BUSYHandle);
        Watchdog_Task_Start();
        return LOG_RW_END_ERR;
    }
    else
    {
        read_log_num = 0;
        lseek_num = 0;
        osMutexRelease(SD_BUSYHandle);
        Watchdog_Task_Start();
        return LOG_RW_OK;
    }
}

/*************************************************
Function: 			Read_SDInfo
Description:		读取SD信息
Input:  none
Return: OK[1] ERR[0]
*************************************************/
uint8_t Read_SDInfo(void)
{
    FIL fil;
    sd_info_t sd_info;

    osMutexWait(SD_BUSYHandle,osWaitForever);

    retSD = f_open(&fil, "SD_info.txt", FA_READ);
    if(retSD != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        return LOG_RW_ERROR;
    }
    retSD = f_read(&fil,&sd_info,sizeof(sd_info_t),&byte_r);
    if(retSD != FR_OK)
    {
        osMutexRelease(SD_BUSYHandle);
        return LOG_RW_ERROR; 
    }
    f_close(&fil);
    osMutexRelease(SD_BUSYHandle);
    return LOG_RW_OK;
}
/*************************************************
Function: 			Log_Init
Description:		Log 初始化
Input:  none
Return: none
*************************************************/


FRESULT scan_files (void)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
    uint32_t file_num = 0;
    char old_filename[14] = {0};
	uint8_t break_cnt = 0;
	LOGFILETIME curlogfiletime,lastlogfiletime;
	
	char error_file[4][20] = 
	{
		"255-255-255.txt",
		"2000-00-00.txt",
		"20255-255-255.txt",
		"00-00-00.txt"
	};
	uint8_t i;
	uint8_t error_file_flag;
	
	osMutexWait(SD_BUSYHandle,osWaitForever);
	res = f_opendir(&dir, "/");
	if (res == FR_OK)
	{
		while(1) 
		{
			res = f_readdir(&dir, &fno);
			break_cnt ++ ;
			if (res != FR_OK || fno.fname[0] == 0) 
			{
				break;
			}
/*error file handle*/			
			error_file_flag = 0;
			for(i=0;i<4;i++)
			{
				if(!strcmp(error_file[i],fno.fname))
				{
					error_file_flag = 1;
				}
			}
			if(error_file_flag)
			{
				f_unlink(fno.fname);
				continue;
			}
			
      //文件名以2开头，代表是日志文件 2019-xx-xx
      if(((fno.fname[0] - 0x30) == 2)&&((fno.fname[2] - 0x30) != 0)&&((fno.fname[3] - 0x30) != 0))
			{
				file_num++;//日志文件数量
				if(file_num == 1)
				{
					memcpy(old_filename,fno.fname,sizeof(old_filename));
				}
				
				curlogfiletime.Year   = (fno.fname[2] - 0x30)*10+(fno.fname[3]- 0x30);
				curlogfiletime.month  = (fno.fname[5] - 0x30)*10+(fno.fname[6]- 0x30);
				curlogfiletime.day    = (fno.fname[8] - 0x30)*10+(fno.fname[9]- 0x30);
				
				lastlogfiletime.Year  = (old_filename[2]- 0x30)*10+(old_filename[3]- 0x30);
				lastlogfiletime.month = (old_filename[5]- 0x30)*10+(old_filename[6]- 0x30);
				lastlogfiletime.day   = (old_filename[8]- 0x30)*10+(old_filename[9]- 0x30);
				
				if(curlogfiletime.Year == lastlogfiletime.Year)
				{
					if(curlogfiletime.month == lastlogfiletime.month)
					{
						if(curlogfiletime.day < lastlogfiletime.day)
						{
							memcpy(old_filename,fno.fname,sizeof(old_filename));
						}						
					}
					else if(curlogfiletime.month < lastlogfiletime.month)
					{
						memcpy(old_filename,fno.fname,sizeof(old_filename));
					}			
				}
				else if(curlogfiletime.Year < lastlogfiletime.Year)
				{
					memcpy(old_filename,fno.fname,sizeof(old_filename));
				}
			}
			osDelay(100);//100ms = 10hz
			if (break_cnt > 200) break;
		}
    
    //日志文件大于30，会删除最旧的一个日志文件
    if(file_num > 30)
		{
      //删除最旧的文件
			res = f_unlink(old_filename);
		}			
	}
	osMutexRelease(SD_BUSYHandle);
	return res;
}
void Log_Init(void)
{
    /* 创建互斥信号量 */
    xSemaphore = xSemaphoreCreateMutex();
    
    //创建SD 信息文件
    creat_file_name(eFILE_SD_INFO);
    //创建 环境变量文件
    creat_file_name(eFILE_ENV);
    env_file_init();
    //创建 日期日志文件
    log_file_status_init();
    creat_file_name(eFILE_TEMP);
    creat_file_name(eFILE_LOG);
}

