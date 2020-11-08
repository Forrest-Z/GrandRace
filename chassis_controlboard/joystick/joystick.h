#ifndef __JOYSTICK_H
#define __JOYSTICK_H
#include "stm32f4xx_hal.h"
#include "OD.h"
#include "math.h"
#include "string.h"
#include "chassis_com.h"

#pragma pack(1)

#define WARN_TH_JOY_MIN				     -100
#define WARN_TH_JOY_MAX			      	100

#define JOYS_STATE_OK 									0  //正常
#define JOYS_STATE_ERR 									1  //异常

//通讯异常 b0000 0010
#define JOYS_DISCONNECT						 		0x02

typedef union
{
    uint8_t   u8_v;
    struct
    {
        __IO uint8_t b00_data_err: 1;     //数据异常
        __IO uint8_t b01_commun_err: 1;   //通讯异常
        __IO uint8_t b02_checksum_err: 1; //原始数据校验错误
        uint8_t b03: 1;
        uint8_t b04: 1;
        uint8_t b05: 1;
        uint8_t b06: 1;
        uint8_t b07: 1;
    } flag;
} eJOYS_STATE;    //sizeof(eJOYS) 为 1




#define JOY_FILT_Ts1							0.1f
#define JOY_FILT_Ts2							0.01f
#define JOY_MAX_V_DEFAULT 				2.22f  //max_speed default
#define JOY_MAX_A_DEFAULT					0.3f	//max_angle default
#define JOY_A1 										1.0f
#define JOY_A1_VALUE 							0.3f
#define STOP_OFFSET_POINT 				0.15f
#define JOY_DEADBAND							0.12f 	//插入摇杆死区

#define JOY_MAX_V 				2.30f //接收最大线速度
#define JOY_MIN_V 				1.0f  //限速最小线速度


#define JOY_MAX_A					1.0f  //接收最大角速度
#define JOY_MIN_A					0.2f  //接收最小角速度

typedef struct
{
    int32_t val_x;
    int32_t val_y;
    int32_t Init_val_x;
    int32_t Init_val_y;
		float pre_x;				//原始值预处理
		float pre_y;
    float offset_val_x;	//经过平滑滤波后的量
    float offset_val_y;	//经过平滑滤波后的量
    float real_x_max;		//真实角速度最大值(rad/s)
    float real_y_max;		//真实线速度最大值(m/s)
		float real_x;				//经过映射后的，有物理意义的量 角速度
		float real_y;				//经过映射后的，有物理意义的量 线速度				
} Joystick_TypeDef;

void initJoystick(void);
float filterAlgorithm(float arg,uint8_t status);
float filterAcc(float arg,uint8_t status);

uint8_t readJoystickDevice(void);
uint8_t getJoystickData(Joystick_TypeDef *pmsg);
float mapToControlCurve(float x);
float stopOffset(float x);
uint8_t getJoystickState(void);
void clearJoystickState(void);




extern float ready_speed_line;
extern float ready_speed_an;

void setJoyMode(uint8_t mod);
uint8_t getJoyMode(void);

#endif
