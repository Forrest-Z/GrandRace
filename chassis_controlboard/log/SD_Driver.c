#include "SD_Driver.h"

sd_info_t SD_INFO;	//get sd card info
#ifdef __TP_USER
#include "fault_injector_wrapper.h"
#endif

/*************************BASE**********************************************/
uint8_t check_SD_exit()
{
    if(HAL_GPIO_ReadPin(SD_EXIT_FLAG_GPIO_Port,SD_EXIT_FLAG_Pin) == 1)
    {
        return SD_EXIT;
    }
    else
    {
        return SD_INSET;
    }
}

float Get_SD_Size(sd_info_t *sd)
{
    if(check_SD_exit())
    {
        osMutexWait(SD_BUSYHandle,osWaitForever);
        DWORD fre_clust,fre_sect,tot_sect;
        FATFS *fis;

        retSD = f_getfree("/",&fre_clust,&fis);
        tot_sect = (fis->n_fatent -2) *fis->csize;
        fre_sect = fre_clust * fis->csize;

#if _MAX_SS!=512
        tot_sect *= fs1->ssize/512;
        fre_sect *= fs1->ssize/512;
#endif
        sd->total_size = (float)(tot_sect >> 1)/1024.0f/1024.0f;
        sd->free_size  = (float)(fre_sect >> 1)/1024.0f/1024.0f;

#ifdef __TP_USER
        TP_MODIFY_VALUE(eTP4_SD_FULL,sd->free_size, SD_INFO.total_size*0.1f);
#endif
        osMutexRelease(SD_BUSYHandle);
        return sd->free_size;
    }
    return 0;
}

/****************************App******************************************/

uint8_t Get_Sdtorage_spacestatus(void)
{
    Get_SD_Size(&SD_INFO);
    if(SD_INFO.free_size<SD_INFO.total_size*0.2f)
    {
        return SD_SPACE_FULL;
    }
    else
    {
        return SD_SPACE_FREE;
    }
}
void SD_App_Init()
{
    if(check_SD_exit())
    {
        MX_FATFS_Init();
        Get_SD_Size(&SD_INFO);
    }
    else
    {
        //通信线程还没起来 怎么能发送？
        // sdlog_printf_app("No SD card!");
    }
}

