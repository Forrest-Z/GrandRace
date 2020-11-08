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
    USER_CMD,//�û���
    ACCT_CMD,//����û��˻���telnet��
    PASS_CMD,//����
    TYPE_CMD,//��ʾ����
    LIST_CMD,//�б�   �����������б�����DTP
    CWD_CMD,//�ı乤��Ŀ¼
    DELE_CMD,//ɾ��   ɾ��ָ��·���µ��ļ�
    NAME_CMD,
    QUIT_CMD,//�˳���¼
    RETR_CMD,//���³�ʼ��
    STOR_CMD,//����  ���������ݱ��浽�������ļ���
    PORT_CMD,//���ݶ˿�
    NLST_CMD,//�����б�   ����������Ŀ¼�������û�
    PWD_CMD,//��ӡ����Ŀ¼
    XPWD_CMD,
    MKD_CMD,//����Ŀ¼
    XMKD_CMD,
    XRMD_CMD,
    RMD_CMD,
    STRU_CMD,//�ļ��ṹ   F-�ļ�  R-��¼�ṹ    P-ҳ�ṹ
    MODE_CMD,//����ģʽ   S-��  B-��  C-ѹ��
    SYST_CMD,//ϵͳ  ����ȷ�������������еĲ���ϵ
    XMD5_CMD,
    XCWD_CMD,
    FEAT_CMD,
    PASV_CMD,//����   ���뱻����������״̬
    SIZE_CMD,
    MLSD_CMD,
    APPE_CMD,//����
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
