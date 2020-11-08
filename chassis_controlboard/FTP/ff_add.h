#ifndef __FF_ADD_H
#define	__FF_ADD_H

#include "ff.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>

extern char Listbuf[2048];

extern osThreadId CommunicationHandle;
extern osThreadId SensorHandle;
extern osThreadId ControlHandle;
extern osThreadId StateMachineHandle;
extern osThreadId LOGTaskHandle;

extern FRESULT Get_FileList(char* path, char *buf, int * buf_len);
extern FRESULT FTP_scan_files(char* path, int * buf_len);
extern int get_filesize(char* path, char *filename);

extern void Suspend_Task(void);
extern void Resume_Task(void);
extern void Suspend_Task_All(void);
extern void Resume_Task_All(void);

#endif

