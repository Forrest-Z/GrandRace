#ifndef _FTPD_H_
#define _FTPD_H_

/*
* Wiznet.
* (c) Copyright 2002, Wiznet.
*
* Filename	: ftpd.h
* Version	: 1.0
* Programmer(s)	:
* Created	: 2015/04/28
* Description   : Header file of FTP daemon. (AVR-GCC Compiler)
*/

#include <stdint.h>

#define F_FILESYSTEM // If your target support a file system, you have to activate this feature and implement.

#ifdef F_FILESYSTEM
//#include "SD_App.h"
//#include "log.h"
#include "SD_Log_App.h"
#endif

#define F_APP_FTP
//#define _FTP_DEBUG_


#define LINELEN		100
//#define DATA_BUF_SIZE	100
#if !defined(F_FILESYSTEM)
#define _MAX_SS		512
#endif

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

#define CTRL_SOCK	2
#define DATA_SOCK	3

#define	IPPORT_FTPD	20	/* FTP Data port */
#define	IPPORT_FTP	21	/* FTP Control port */

#define HOSTNAME	"W5500"
#define VERSION		"1.0"

#define FILENAME	"a.txt"

extern uint8_t test_task;
extern uint8_t del_flag;

enum
{
    FTP_CLOSED = 0,
    FTP_CLOSE_WAIT,
    FTP_INIT,
    FTP_LISTEN,
    FTP_ESTABLISHED
};

extern uint8_t FTP_Status;
extern uint8_t FTP_TimeOut;

/* FTP commands */
enum ftp_cmd {
    USER_CMD,//用户名
    ACCT_CMD,//标记用户账户的telnet串
    PASS_CMD,//口令
    TYPE_CMD,//表示类型
    LIST_CMD,//列表   服务器传送列表到被动DTP
    CWD_CMD,//改变工作目录
    DELE_CMD,//删除   删除指定路径下的文件
    NAME_CMD,
    QUIT_CMD,//退出登录
    RETR_CMD,//重新初始化
    STOR_CMD,//保存  将接受数据保存到服务器文件在
    PORT_CMD,//数据端口
    NLST_CMD,//名字列表   服务器传送目录表名到用户
    PWD_CMD,//打印工作目录
    XPWD_CMD,
    MKD_CMD,//创建目录
    XMKD_CMD,
    XRMD_CMD,
    RMD_CMD,
    STRU_CMD,//文件结构   F-文件  R-记录结构    P-页结构
    MODE_CMD,//传输模式   S-流  B-块  C-压缩
    SYST_CMD,//系统  用于确定服务器上运行的操作系
    XMD5_CMD,
    XCWD_CMD,
    FEAT_CMD,
    PASV_CMD,//被动   进入被动接受请求状态
    SIZE_CMD,
    MLSD_CMD,
    APPE_CMD,//附加
    NO_CMD,
};

enum ftp_type
{
    ASCII_TYPE,
    IMAGE_TYPE,
    LOGICAL_TYPE
};

enum ftp_state
{
    FTPS_NOT_LOGIN,
    FTPS_LOGIN
};

enum datasock_state
{
    DATASOCK_IDLE,
    DATASOCK_READY,
    DATASOCK_START
};

enum datasock_mode
{
    PASSIVE_MODE,
    ACTIVE_MODE
};

struct ftpd
{
    uint8_t control;			/* Control stream */
    uint8_t data;			/* Data stream */

    enum ftp_type type;		/* Transfer type */
    enum ftp_state state;

    enum ftp_cmd current_cmd;

    enum datasock_state dsock_state;
    enum datasock_mode dsock_mode;

    char username[LINELEN];		/* Arg to USER command */
    char workingdir[LINELEN];
    char filename[LINELEN];

#if defined(F_FILESYSTEM)
    FIL fil;	// FatFs File objects
    FRESULT fr;	// FatFs function common result code
#endif

};

void ftpd_init(uint8_t * src_ip);
uint8_t ftpd_run(uint8_t * dbuf);
char proc_ftpd(char * buf);
char ftplogin(char * pass);
int pport(char * arg);

int sendit(char * command);
int recvit(char * command);

long sendfile(uint8_t s, char * command);
long recvfile(uint8_t s);

#if defined(F_FILESYSTEM)
void print_filedsc(FIL *fil);
#endif
#endif // _FTPD_H_
