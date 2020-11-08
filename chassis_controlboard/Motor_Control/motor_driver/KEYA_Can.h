#ifndef _KEYA_H
#define _KEYA_H

#include "control_config.h"
#include "can.h"

#define KEYA_ADDR_LEFT  1
#define KEYA_ADDR_RIGHT 2
//三次之后再上告警
#define KEYA_LOG_MAX_CNT 					  3
#define KEYA_LOG_OUT 					      4

//左右电机类别项
typedef enum _eMOTOR_LR
{
    eM_L = 0, //左电机
    eM_R = 1,		  //右电机
} eMOTOR_LR;

#define ERR_F1_KEYA 0x01		//过热 
#define ERR_F2_KEYA 0x02		//过压
#define ERR_F3_KEYA 0x04		//欠压
#define ERR_F4_KEYA 0x08		//短路
#define ERR_F5_KEYA 0x10		//紧急停止10
#define ERR_F6_KEYA 0x20		//无刷传感器故障11
#define ERR_F7_KEYA 0x40		//MOSFET故障12
#define ERR_F8_KEYA 0x80		//启动时加载的默认配置14


void init_KEYA(MOTORCONTROL_Typedef *motor_arg);

void writeSpeed_KEYA(float pwm,unsigned char addr);

float readSpeed_KEYA(unsigned char addr);
float readDistance_KEYA(unsigned char addr);
float readElec_KEYA(unsigned char addr);
float readVolt_KEYA(void);
unsigned short readErr_KEYA(void);

/*回调函数
**************************************************/
void canCallback_KEYA(uint8_t data[8]);
void speedCallback_KEYA(uint8_t data[8]);
void errCallback_KEYA(uint8_t data[8]);
void elecCallback(uint8_t data[8]);
void voltCallback(uint8_t data[8]);
void keya_stop(void);
/*end*************************************************/

void check_driver_short_circuit(uint16_t *buf,uint8_t err_L,uint8_t err_R);

#endif

