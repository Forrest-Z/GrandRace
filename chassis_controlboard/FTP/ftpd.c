#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include "socket.h"
#include "w5500.h"
#include "ftpd.h"
#include "util.h"
#include "utility.h"
#include "ff_add.h"
#include "tcp_demo.h"

/* Command table */
static char *commands[] = {
    "user",
    "acct",
    "pass",
    "type",
    "list",
    "cwd",
    "dele",
    "name",
    "quit",
    "retr",
    "stor",
    "port",
    "nlst",
    "pwd",
    "xpwd",
    "mkd",
    "xmkd",
    "xrmd",
    "rmd ",
    "stru",
    "mode",
    "syst",
    "xmd5",
    "xcwd",
    "feat",
    "pasv",
    "size",
    "mlsd",
    "appe",
    NULL
};


#define FLASH_PAGE_SIZE    ((uint16_t)0x800)

static un_l2cval remote_ip;
static uint16_t  remote_port;
static un_l2cval local_ip;
static uint16_t  local_port;
uint8_t connect_state_control = 0;
uint8_t connect_state_data = 0;
extern int32_t Filenum;
uint8_t del_flag = 0;

struct ftpd ftp;
uint8_t FTP_Status;
uint8_t FTP_TimeOut;

int current_year = 2015;
int current_month = 4;
int current_day = 27;
int current_hour = 10;
int current_min = 10;
int current_sec = 30;

uint32_t tick_Start;
FRESULT res;

uint32_t read_time;
uint8_t sd_error_flag;

int fsprintf(uint8_t s, const char *format, ...)
{
    /*
        int i;

    char buf[LINELEN];
    FILE f;
    va_list ap;

    f.flags = __SWR | __SSTR;
    f.buf = buf;
    f.size = INT_MAX;
    va_start(ap, format);
    i = vfprintf(&f, format, ap);
    va_end(ap);
    buf[f.len] = 0;

    send(s, (uint8_t *)buf, strlen(buf));

    return i;
        */

    return 0;
}

void ftpd_init(uint8_t * src_ip)
{
    ftp.state = FTPS_NOT_LOGIN;
    ftp.current_cmd = NO_CMD;
    ftp.dsock_mode = ACTIVE_MODE;

    local_ip.cVal[0] = src_ip[0];
    local_ip.cVal[1] = src_ip[1];
    local_ip.cVal[2] = src_ip[2];
    local_ip.cVal[3] = src_ip[3];
    local_port = 35000;

    strcpy(ftp.workingdir, "/");

    socket(CTRL_SOCK, Sn_MR_TCP, IPPORT_FTP, 0x0);
}

uint8_t ftpd_run(uint8_t * dbuf)
{
    uint16_t size = 0;
    long ret = 0;
    uint32_t blocklen, send_byte;
//    uint32_t recv_byte;
    uint32_t remain_filesize;

#if defined(_FTP_DEBUG_)
    float filesize;
#endif

//    uint32_t remain_datasize;

//    int16_t fno=-1;

#if defined(F_FILESYSTEM)
    //FILINFO fno;
#endif

    switch(getSn_SR(CTRL_SOCK))
    {
    case SOCK_ESTABLISHED :
        if(!connect_state_control)
        {
#if defined(_FTP_DEBUG_)
            printf("%d:FTP Connected\r\n", CTRL_SOCK);
#endif
            //fsprintf(CTRL_SOCK, banner, HOSTNAME, VERSION);
            strcpy(ftp.workingdir, "/");
            sprintf((char *)dbuf, "220 %s FTP version %s ready.\r\n", HOSTNAME, VERSION);
            ret = send(CTRL_SOCK, (uint8_t *)dbuf, strlen((const char *)dbuf));
            if(ret < 0)
            {
#if defined(_FTP_DEBUG_)
                printf("%d:send() error:%ld\r\n",CTRL_SOCK,ret);
#endif
                close(CTRL_SOCK);
                return ret;
            }
            connect_state_control = 1;
        }
        if((size = getSn_RX_RSR(CTRL_SOCK)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't not occur.
        {
#if defined(_FTP_DEBUG_)
//             printf("size: %d\r\n", size);
#endif
            memset(dbuf, 0, _MAX_SS);
            if(size > _MAX_SS) size = _MAX_SS - 1;
            ret = recv(CTRL_SOCK,dbuf,size);
            dbuf[ret] = '\0';
            if(ret != size)
            {
                if(ret==SOCK_BUSY) return 0;
                if(ret < 0)
                {
#if defined(_FTP_DEBUG_)
                    printf("%d:recv() error:%ld\r\n",CTRL_SOCK,ret);
#endif
                    close(CTRL_SOCK);
                    return ret;
                }
            }
#if defined(_FTP_DEBUG_)
            printf("Rcvd Command:   %s", dbuf);
#endif
            proc_ftpd((char *)dbuf);   //Parsing Commands
        }
        FTP_Status = FTP_ESTABLISHED;
        if((tcp_connectflag == 0) ||(del_flag == 0xFF))  //if heart over, close ctrl_cock
        {
            disconnect(CTRL_SOCK);
            close(CTRL_SOCK);
        }
        break;
    case SOCK_CLOSE_WAIT :
#if defined(_FTP_DEBUG_)
        printf("%d:CloseWait\r\n",CTRL_SOCK);
#endif
        disconnect(CTRL_SOCK);
#if defined(_FTP_DEBUG_)
        printf("%d:Closed\r\n",CTRL_SOCK);
#endif
        FTP_Status = FTP_CLOSE_WAIT;
        break;
    case SOCK_CLOSED :
#if defined(_FTP_DEBUG_)
        printf("%d:FTPStart\r\n",CTRL_SOCK);
#endif
        FTP_Status = FTP_CLOSED;
        if((ret=socket(CTRL_SOCK, Sn_MR_TCP, IPPORT_FTP, 0x0)) !=SOCK_OK )
        {
#if defined(_FTP_DEBUG_)
            printf("%d:socket() error:%ld\r\n", CTRL_SOCK, ret);
#endif
            close(CTRL_SOCK);
            return ret;
        }
        break;
    case SOCK_INIT :
#if defined(_FTP_DEBUG_)
        printf("%d:Opened\r\n",CTRL_SOCK);
#endif
        //strcpy(ftp.workingdir, "/");

        FTP_Status = FTP_INIT;
        FTP_TimeOut = 0;
        tick_Start = xTaskGetTickCount();
        if( (ret = listen(CTRL_SOCK)) != SOCK_OK)
        {
#if defined(_FTP_DEBUG_)
            printf("%d:Listen error\r\n",CTRL_SOCK);
#endif
            return ret;
        }
        connect_state_control = 0;
#if defined(_FTP_DEBUG_)
        printf("%d:Listen ok\r\n",CTRL_SOCK);
#endif
        break;
    case SOCK_LISTEN:
        FTP_Status = FTP_LISTEN;
        if(xTaskGetTickCount() - tick_Start > 10000)
        {
            FTP_TimeOut = 0xFF;
        }
        break;
    default :
        break;
    }


    switch(getSn_SR(DATA_SOCK))
    {
    case SOCK_ESTABLISHED :

        if(!connect_state_data)
        {
#if defined(_FTP_DEBUG_)
            printf("%d:FTP Data socket Connected\r\n", DATA_SOCK);
#endif
            connect_state_data = 1;
        }
        switch(ftp.current_cmd)
        {
        case LIST_CMD://列表   服务器传送列表到被动DTP
        case MLSD_CMD:
#if defined(_FTP_DEBUG_)
            printf("previous size: %d\r\n", size);
#endif
#if defined(F_FILESYSTEM)
            osMutexWait(SD_BUSYHandle,osWaitForever);
            res = FTP_scan_files(ftp.workingdir,(int *)&size);
            osMutexRelease(SD_BUSYHandle);
#endif
#if defined(_FTP_DEBUG_)
            printf("returned size: %d\r\n", size);
            printf("%s\r\n", dbuf);
#endif
#if !defined(F_FILESYSTEM)
            //if (strncmp(ftp.workingdir, "/$Recycle.Bin", sizeof("/$Recycle.Bin")) != 0)
            //size = sprintf((char *)dbuf, "drwxr-xr-x 1 ftp ftp 0 Dec 31 2014 $Recycle.Bin\r\n-rwxr-xr-x 1 ftp ftp 512 Dec 31 2014 test.txt\r\n");

            for(i=0; i<Filenum+1; i++)
            {
                size = sprintf((char *)dbuf, "-rwxr-xr-x 1 ftp ftp 80 May 1 2015 %04d.txt\r\n",i+1);
                size = strlen((char const *)dbuf);
                send(DATA_SOCK, dbuf, size);
            }

#else
            size = strlen((char const *)Listbuf);  //dispaly filenames // care
            send(DATA_SOCK,(uint8_t *)Listbuf, size);
            memset(Listbuf, 0, size);
#endif
            ftp.current_cmd = NO_CMD;
            disconnect(DATA_SOCK);
            if(res == FR_OK)
                size = sprintf((char *)dbuf, "226 Successfully transferred \"%s\"\r\n", ftp.workingdir);
            else
                size = sprintf((char *)dbuf, "550 read list Error  \"%s\"\r\n", ftp.workingdir);
            send(CTRL_SOCK, dbuf, size);
            break;
        case RETR_CMD:
#if defined(_FTP_DEBUG_)
            printf("\nfilename to retrieve : %s\r\n", ftp.filename);
#endif
#if defined(F_FILESYSTEM)
            read_time = 0;
            osMutexWait(SD_BUSYHandle,osWaitForever);
            vTaskSuspend(CommunicationHandle);
            ftp.fr = f_open(&(ftp.fil), (const char *)ftp.filename, FA_READ);
            vTaskResume(CommunicationHandle);
            osMutexRelease(SD_BUSYHandle);
            if(ftp.fr == FR_OK)
            {
                remain_filesize = f_size(&(ftp.fil));
#if defined(_FTP_DEBUG_)
                filesize = remain_filesize;
                printf("begin...\r\n");
#endif
                do
                {
                    test_task = 6;

                    memset(dbuf, 0, _MAX_SS);
                    if(remain_filesize > _MAX_SS)
                        send_byte = _MAX_SS;
                    else
                        send_byte = remain_filesize;

                    osMutexWait(SD_BUSYHandle,osWaitForever);
                    vTaskSuspend(CommunicationHandle);
//								Suspend_Task_All();
                    ftp.fr = f_read(&(ftp.fil), dbuf, send_byte, &blocklen);
//								Resume_Task_All();
                    vTaskResume(CommunicationHandle);
                    osMutexRelease(SD_BUSYHandle);
                    if(ftp.fr != FR_OK)
                    {
                        sd_error_flag = 2;
#if defined(_FTP_DEBUG_)
                        printf("read file error\n");
#endif
                        //ftp.fr = f_close(&(ftp.fil));
                        break;
                    }

                    if((tcp_connectflag == 0)||(del_flag == 0xFF)) //if heart over, quit read data
                    {
                        break;
                    }

#if defined(_FTP_DEBUG_)
                    printf(" %d%%\n",(uint32_t)((filesize-remain_filesize)*100/filesize));
#endif
                    send(DATA_SOCK, dbuf, blocklen);
                    read_time += blocklen;
                    remain_filesize -= blocklen;
                    Watchdog_Task_Feed(TASK_BIT_0 | TASK_BIT_1| TASK_BIT_4);
                } while(remain_filesize != 0);
#if defined(_FTP_DEBUG_)
                printf("%d%%\n",(uint32_t)((filesize-remain_filesize)*100/filesize));
                printf("File read finished\n\n");
#endif
                osMutexWait(SD_BUSYHandle,osWaitForever);
                ftp.fr = f_close(&(ftp.fil));
                osMutexRelease(SD_BUSYHandle);
                if(ftp.fr != FR_OK)
                {
                    sd_error_flag = 3;
                }
            }
            else
            {
                sd_error_flag = 1;
#if defined(_FTP_DEBUG_)
                printf("File Open Error: %d\r\n", ftp.fr);
#endif
            }
#else   //FLASH mode          
            memset(dbuf, 0, _MAX_SS);
            fno=(*(ftp.filename+2)-0x30)*1000+(*(ftp.filename+3)-0x30)*100+(*(ftp.filename+4)-0x30)*10+(*(ftp.filename+5)-0x30);
            fno--;
            memcpy( dbuf, (void const *)(0x08020000+(fno/25)*FLASH_PAGE_SIZE+(fno%25)*80), 80);
            printf("File Open No.: %d, %c, %c, %c, %c, %c, %c, %c, %c.\r\n", fno, dbuf[0], dbuf[1], dbuf[2], dbuf[3], dbuf[4], dbuf[5], dbuf[6], dbuf[7]);
            send(DATA_SOCK, dbuf, 80);
#endif
            ftp.current_cmd = NO_CMD;
            disconnect(DATA_SOCK);
            size = sprintf((char *)dbuf, "226 Successfully transferred \"%s\"\r\n", ftp.filename);
            send(CTRL_SOCK, dbuf, size);
            break;
        case STOR_CMD:
            break;
        case NO_CMD:
        default:
            break;
        }
        break;

    case SOCK_CLOSE_WAIT :
#if defined(_FTP_DEBUG_)
        printf("%d:CloseWait\r\n",DATA_SOCK);
#endif
        disconnect(DATA_SOCK);

#if defined(_FTP_DEBUG_)
        printf("%d:Closed\r\n",DATA_SOCK);
#endif
        break;
    case SOCK_CLOSED :
        if(ftp.dsock_state == DATASOCK_READY)
        {
            if(ftp.dsock_mode == PASSIVE_MODE)
            {
#if defined(_FTP_DEBUG_)
                printf("%d:FTPDataStart, port : %d\r\n",DATA_SOCK, local_port);
#endif
                if((ret=socket(DATA_SOCK, Sn_MR_TCP, local_port, 0x0)) !=SOCK_OK)
                {
#if defined(_FTP_DEBUG_)
                    printf("%d:socket() error:%ld\r\n", DATA_SOCK, ret);
#endif
                    close(DATA_SOCK);
                    return ret;
                }
//            local_port++;    //jck
                if(local_port > 50000)
                    local_port = 35000;
            }
            else
            {
#if defined(_FTP_DEBUG_)
                printf("%d:FTPDataStart, port : %d\r\n",DATA_SOCK, IPPORT_FTPD);
#endif
                if((ret=socket(DATA_SOCK, Sn_MR_TCP, IPPORT_FTPD, 0x0)) != SOCK_OK)
                {
#if defined(_FTP_DEBUG_)
                    printf("%d:socket() error:%ld\r\n", DATA_SOCK, ret);
#endif
                    close(DATA_SOCK);
                    return ret;
                }
            }
            ftp.dsock_state = DATASOCK_START;
        }
        break;

    case SOCK_INIT :
#if defined(_FTP_DEBUG_)
        printf("%d:Opened\r\n",DATA_SOCK);
#endif
        if(ftp.dsock_mode == PASSIVE_MODE)
        {
            if( (ret = listen(DATA_SOCK)) != SOCK_OK)
            {
#if defined(_FTP_DEBUG_)
                printf("%d:Listen error\r\n",DATA_SOCK);
#endif
                return ret;
            }
#if defined(_FTP_DEBUG_)
            printf("%d:Listen ok\r\n",DATA_SOCK);
#endif
        }
        else
        {
            if((ret = connect(DATA_SOCK, remote_ip.cVal, remote_port)) != SOCK_OK)
            {
#if defined(_FTP_DEBUG_)
                printf("%d:Connect error\r\n", DATA_SOCK);
#endif
                return ret;
            }
        }
        connect_state_data = 0;
        break;

    default :
        break;
    }       //end switch
    return 0;
}         //end ftpd_run();

char proc_ftpd(char * buf)
{
    char **cmdp, *cp, *arg, *tmpstr;
    char sendbuf[200];
    int slen;
    long ret;


    /* Translate first word to lower case */
    for (cp = buf; *cp != ' ' && *cp != '\0'; cp++)
        *cp = tolower(*cp);

    /* Find command in table; if not present, return syntax error */
    for (cmdp = commands; *cmdp != NULL; cmdp++)
        if (strncmp(*cmdp, buf, strlen(*cmdp)) == 0)
            break;

    if (*cmdp == NULL)
    {
        //fsprintf(CTRL_SOCK, badcmd, buf);
        slen = sprintf(sendbuf, "500 Unknown command '%s'\r\n", buf);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        return 0;
    }
    /* Allow only USER, PASS and QUIT before logging in */
    if (ftp.state == FTPS_NOT_LOGIN)
    {
        switch(cmdp - commands)
        {
        case USER_CMD:
        case PASS_CMD:
        case QUIT_CMD:
            break;
        default:
            //fsprintf(CTRL_SOCK, notlog);
            slen = sprintf(sendbuf, "530 Please log in with USER and PASS\r\n");
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
            return 0;
        }
    }

    arg = &buf[strlen(*cmdp)];
    while(*arg == ' ') arg++;

    /* Execute specific command */
    switch (cmdp - commands)
    {
    case USER_CMD :
#if defined(_FTP_DEBUG_)
        printf("USER_CMD : %s", arg);
#endif
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
        strcpy(ftp.username, arg);
        //fsprintf(CTRL_SOCK, givepass);
        slen = sprintf(sendbuf, "331 Enter PASS command\r\n");
        ret = send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        if(ret < 0)
        {
#if defined(_FTP_DEBUG_)
            printf("%d:send() error:%ld\r\n",CTRL_SOCK,ret);
#endif
            close(CTRL_SOCK);
            return ret;
        }
        break;

    case PASS_CMD :
#if defined(_FTP_DEBUG_)
        printf("PASS_CMD : %s", arg);
#endif
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
        ftplogin(arg);
        break;

    case TYPE_CMD :
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
        switch(arg[0])
        {
        case 'A':
        case 'a':	/* Ascii */
            ftp.type = ASCII_TYPE;
            //fsprintf(CTRL_SOCK, typeok, arg);
            slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
            break;

        case 'B':
        case 'b':	/* Binary */
        case 'I':
        case 'i':	/* Image */
            ftp.type = IMAGE_TYPE;
            //fsprintf(CTRL_SOCK, typeok, arg);
            slen = sprintf(sendbuf, "200 Type set to %s\r\n", arg);
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
            break;

        default:	/* Invalid */
            //fsprintf(CTRL_SOCK, badtype, arg);
            slen = sprintf(sendbuf, "501 Unknown type \"%s\"\r\n", arg);
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
            break;
        }
        break;

    case FEAT_CMD :
        slen = sprintf(sendbuf, "211-Features:\r\n MDTM\r\n REST STREAM\r\n SIZE\r\n MLST size*;type*;create*;modify*;\r\n MLSD\r\n UTF8\r\n CLNT\r\n MFMT\r\n211 END\r\n");
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case QUIT_CMD :
#if defined(_FTP_DEBUG_)
        printf("QUIT_CMD\r\n");
#endif
        //fsprintf(CTRL_SOCK, bye);
        slen = sprintf(sendbuf, "221 Goodbye!\r\n");
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        disconnect(CTRL_SOCK);
        break;

    case RETR_CMD :
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
#if defined(_FTP_DEBUG_)
        printf("RETR_CMD\r\n");
#endif
        if(strlen(ftp.workingdir) == 1)
            sprintf(ftp.filename, "/%s", arg);
        else
            sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
        slen = sprintf(sendbuf, "150 Opening data channel for file downloand from server of \"%s\"\r\n", ftp.filename);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        ftp.current_cmd = RETR_CMD;

        break;

    case APPE_CMD :
    case STOR_CMD:
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
#if defined(_FTP_DEBUG_)
        printf("STOR_CMD\r\n");
#endif
        if(strlen(ftp.workingdir) == 1)
            sprintf(ftp.filename, "/%s", arg);
        else
            sprintf(ftp.filename, "%s/%s", ftp.workingdir, arg);
				
        slen = sprintf(sendbuf, "150 Opening data channel for file upload to server of \"%s\"\r\n", ftp.filename);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        ftp.current_cmd = STOR_CMD;
        if((ret = connect(DATA_SOCK, remote_ip.cVal, remote_port)) != SOCK_OK)
        {
#if defined(_FTP_DEBUG_)
            printf("%d:Connect error\r\n", DATA_SOCK);
#endif
            return ret;
        }
        connect_state_data = 0;
        break;

    case PORT_CMD:
#if defined(_FTP_DEBUG_)
        printf("PORT_CMD\r\n");
#endif
        if (pport(arg) == -1)
        {
            //fsprintf(CTRL_SOCK, badport);
            slen = sprintf(sendbuf, "501 Bad port syntax\r\n");
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        }
        else
        {
            //fsprintf(CTRL_SOCK, portok);
            ftp.dsock_mode = ACTIVE_MODE;
            ftp.dsock_state = DATASOCK_READY;
            slen = sprintf(sendbuf, "200 PORT command successful.\r\n");
            send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        }
        break;

    case MLSD_CMD:
#if defined(_FTP_DEBUG_)
        printf("MLSD_CMD\r\n");
#endif
        slen = sprintf(sendbuf, "150 Opening data channel for directory listing of \"%s\"\r\n", ftp.workingdir);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        ftp.current_cmd = MLSD_CMD;
        break;

    case LIST_CMD:
#if defined(_FTP_DEBUG_)
        printf("LIST_CMD\r\n");
#endif
        slen = sprintf(sendbuf, "150 Opening data channel for directory listing of \"%s\"\r\n", ftp.workingdir);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        ftp.current_cmd = LIST_CMD;
        break;

    case NLST_CMD:
#if defined(_FTP_DEBUG_)
        printf("NLST_CMD\r\n");
#endif
        break;

    case SYST_CMD:
        slen = sprintf(sendbuf, "215 UNIX emulated by WIZnet\r\n");
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case PWD_CMD:
    case XPWD_CMD:
        slen = sprintf(sendbuf, "257 \"%s\" is current directory.\r\n", ftp.workingdir);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case PASV_CMD:
        slen = sprintf(sendbuf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n", local_ip.cVal[0], local_ip.cVal[1], local_ip.cVal[2], local_ip.cVal[3], local_port >> 8, local_port & 0x00ff);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        disconnect(DATA_SOCK);

        ftp.dsock_mode = PASSIVE_MODE;
        ftp.dsock_state = DATASOCK_READY;
#if defined(_FTP_DEBUG_)
        printf("PASV port: %d\r\n", local_port);
#endif
        break;

    case SIZE_CMD:
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
        if(slen > 3)
        {
            tmpstr = strrchr(arg, '/');
            *tmpstr = 0;
#if defined(F_FILESYSTEM)
            slen = get_filesize(arg, tmpstr + 1);
#else
            slen = _MAX_SS;
#endif
            if(slen > 0)
                slen = sprintf(sendbuf, "213 %d\r\n", slen);
            else
                slen = sprintf(sendbuf, "550 File not Found\r\n");
        }
        else
        {
            slen = sprintf(sendbuf, "550 File not Found\r\n");
        }
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case CWD_CMD:
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
        if(slen > 3)
        {
            arg[slen - 3] = 0x00;
            tmpstr = strrchr(arg, '/');
            *tmpstr = 0;
#if defined(F_FILESYSTEM)
            slen = get_filesize(arg, tmpstr + 1);
#else
            slen = 0;
#endif
            *tmpstr = '/';
            if(slen == 0) {
                slen = sprintf(sendbuf, "213 %d\r\n", slen);
                strcpy(ftp.workingdir, arg);
                slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
            }
            else
            {
                slen = sprintf(sendbuf, "550 CWD failed. \"%s\"\r\n", arg);
            }
        }
        else
        {
            strcpy(ftp.workingdir, arg);
            slen = sprintf(sendbuf, "250 CWD successful. \"%s\" is current directory.\r\n", ftp.workingdir);
        }
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case MKD_CMD:
    case XMKD_CMD:
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
#if defined(F_FILESYSTEM)
        if (f_mkdir(arg) != 0)
        {
            slen = sprintf(sendbuf, "550 Can't create directory. \"%s\"\r\n", arg);
        }
        else
        {
            slen = sprintf(sendbuf, "257 MKD command successful. \"%s\"\r\n", arg);
            //strcpy(ftp.workingdir, arg);
        }
#else
        slen = sprintf(sendbuf, "550 Can't create directory. Permission denied\r\n");
#endif
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    case DELE_CMD:
        slen = strlen(arg);
        arg[slen - 1] = 0x00;
        arg[slen - 2] = 0x00;
#if defined(F_FILESYSTEM)
        if (f_unlink(arg) != 0)
        {
            slen = sprintf(sendbuf, "550 Could not delete. \"%s\"\r\n", arg);
        }
        else
        {
            slen = sprintf(sendbuf, "250 Deleted. \"%s\"\r\n", arg);
        }
#else
        slen = sprintf(sendbuf, "550 Could not delete. Permission denied\r\n");
#endif
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

				case XCWD_CMD:
				case ACCT_CMD:
				case XRMD_CMD:
				case RMD_CMD:
				case STRU_CMD:
				case MODE_CMD:
				case XMD5_CMD:
        //fsprintf(CTRL_SOCK, unimp);
        slen = sprintf(sendbuf, "502 Command does not implemented yet.\r\n");
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;

    default:	/* Invalid */
        //fsprintf(CTRL_SOCK, badcmd, arg);
        slen = sprintf(sendbuf, "500 Unknown command \'%s\'\r\n", arg);
        send(CTRL_SOCK, (uint8_t *)sendbuf, slen);
        break;
    }

    return 1;
}


char ftplogin(char * pass)    //password function
{
    char sendbuf[100];
    int slen = 0;

    char password[] = "admin";

    //memset(sendbuf, 0, DATA_BUF_SIZE);

#if defined(_FTP_DEBUG_)
    printf("%s logged in\r\n", ftp.username);
#endif
    //fsprintf(CTRL_SOCK, logged);
    if(strcmp(password,pass))
    {
        slen = sprintf(sendbuf, "530 wrong password\r\n");
        ftp.state = FTPS_NOT_LOGIN;
    }
    else
    {
        slen = sprintf(sendbuf, "230 Logged on\r\n");
        ftp.state = FTPS_LOGIN;
    }
    send(CTRL_SOCK, (uint8_t *)sendbuf, slen);


    return 1;
}

int pport(char * arg)        //get remote ip address and ports
{
    int i;
    char* tok=0;

    for (i = 0; i < 4; i++)
    {
        if(i==0) tok = strtok(arg,",\r\n");
        else	 tok = strtok(NULL,",");
        remote_ip.cVal[i] = (uint8_t)ATOI(tok, 10);   //jck
        if (!tok)
        {
#if defined(_FTP_DEBUG_)
            printf("bad pport : %s\r\n", arg);
#endif
            return -1;
        }
    }
    remote_port = 0;
    for (i = 0; i < 2; i++)
    {
        tok = strtok(NULL,",\r\n");
        remote_port <<= 8;
        remote_port += ATOI(tok, 10);
        if (!tok)
        {
#if defined(_FTP_DEBUG_)
            printf("bad pport : %s\r\n", arg);
#endif
            return -1;
        }
    }
#if defined(_FTP_DEBUG_)
    printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip.cVal[0], remote_ip.cVal[1], remote_ip.cVal[2], remote_ip.cVal[3], remote_port);
#endif

    return 0;
}

#if defined(F_FILESYSTEM)
/*
void print_filedsc(FIL *fil)
{
#if defined(_FTP_DEBUG_)
	printf("File System pointer : %08X\r\n", fil->fs);
	printf("File System mount ID : %d\r\n", fil->id);
	printf("File status flag : %08X\r\n", fil->flag);
	printf("File System pads : %08X\r\n", fil->err);
	printf("File read write pointer : %08X\r\n", fil->fptr);
	printf("File size : %08X\r\n", fil->fsize);
	printf("File start cluster : %08X\r\n", fil->sclust);
	printf("current cluster : %08X\r\n", fil->clust);
	printf("current data sector : %08X\r\n", fil->dsect);
	printf("dir entry sector : %08X\r\n", fil->dir_sect);
	printf("dir entry pointer : %08X\r\n", fil->dir_ptr);
#endif
}
*/
#endif
