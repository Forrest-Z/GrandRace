#ifndef _SYS_MONITOR_H_
#define _SYS_MONITOR_H_
#include <stdint.h>
#include "sys_stack.h"

#pragma pack(1)

#define SENSOR_RESET_ENABLE									1
#define SENSOR_RESET_DISENABLE							0

#define LOG_WORK_OK													0x00
#define LOG_STORAGE_FULL										0x01
#define LOG_BLOCK_BREAK											0x02

#define SYSTEM_WORK_NORMAL							0						// 系统工作正常	
#define SYSTEM_WORK_ANNORMAL						1						// 系统工作异常

typedef struct
{
    Sys_Stack_TypeDef stack;
    uint8_t	    	cpu_usage;		//CPU 使用率
    uint8_t   		wdg_state;		//看门狗状态
    uint32_t 			bms;					//电池状态
    uint16_t 			motor;				//电机状态
    uint16_t 			motor_driver;	//电驱状态
    uint8_t 			joystick;			//摇杆状态
    uint8_t 			imu; 					//imu状态
    uint16_t		  ultrasonic;		//超声波状态
    uint8_t 			weighing;			//重量状态
    uint8_t 			flash;				//flash状态
    uint8_t		    comm_net;     //通信状态
    uint8_t		    led;          //led 状态
    uint8_t		    m4_temp;      //CPU内部温度估值
    uint8_t		    imu_temp;     //imu内部温度估值
    uint8_t		    other_res[6]; //预留
} System_State_TypeDef;

extern System_State_TypeDef system_state;

void initSystemMonitor(void);
void readSystemMonitor(void);


void getSystemMonitor(System_State_TypeDef *pmsg);
void ClearWatchdogStateMonitor(void);
uint8_t getSystemWorkState(void);
void clearCmdVelTimeOut(void);

void checkUltAlarm(uint16_t ultrasonic_state,
                   uint16_t lastUlt);


#endif




