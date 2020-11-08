/****************************************************
 * @file     chassis_com.h
 * @brief    底盘公共函数接口，自实现。
 * @version  V2.0
 * @date     2019-05-31
 * @author   ycy
 ***************************************************/
#ifndef __CHASSIS_COM_H__
#define __CHASSIS_COM_H__
#include "types.h"
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#pragma pack(1)

#define OFF 0  //关闭开关
#define ON  1  //打开开关

#define OK   0
#define ERR  1


/*==================外部公共接口函数声明====================*/
bool isdDigist( const char *pStr);
//指定的某一位数置1
#define SetBit(x, y)   (x |= (1<<y))
//指定的某一位数置0
#define ClearBit(x, y) (x &= ~(1<<y))
//指定的某一位数取反
#define ReverseBit(x,y) (x^=(1<<y))
//获取的某一位的值
#define GetBit(x, y)   ((x>>y) & 1)
//指定的某一位数置为指定的0或1
#define WriteBit(data, position, flag) (flag ? SetBit(data, position) : ClearBit(data, position))

//获取系统版本号信息 打印日志到
void printfChassisVersion(void);
//解析字符串 针对整型数据
bool token_cmd(const char* _cmd_i,const char* _cmd_o,
               uint8 *_out_v,bool _bRead);
//解析字符串 针对浮点型数据
bool token_cmd_f(const char* _cmd_i,const char* _cmd_o,
                 float *_out_f,bool _bRead);

//解析字符串 针对解析两个整形整型数据
bool token_value(const char* _cmd_i,const char* _cmd_o,
                 uint8 *_out_v1,uint32 *_out_v2);

/*==================私有内部接口函数声明====================*/

/*-----------------配置环境------------------------------*/
typedef union
{
    float data;
    char  u8_v[4];
} un_float;

typedef union
{
    un_float f_data[4];
    char c_data[20];	//第17个字节作为停止位
} IMU_FLASH_BUF;

//外部传感器告警名称选项
typedef enum
{
    eMOTOR    = 0, 	 //电机
    eDRIVER   = 1,   //电驱
    eCHASSIS  = 2,  //底盘
} eCHASSIS_TYPE;

typedef union _CHASSIS_CONFIG
{
    uint32_t  env_chassis;
    uint8_t   buff[4];//4
    struct
    {
        uint8_t motor_type; 	//[MOTOR_ORIGINAL=1] [MOTOR_ZHAOYANG=2] [MOTOR_JIQIN90N=3][MOTOR_LIANYI60N=4  MOTOR_LIANYI_NEW 5 ] 1
        uint8_t driver_type; 	//电驱定义  [BLD_IO=1] [AQMDBL=2] [KEYA=3] 1
        uint8_t chassis_type; //底盘定义  [WHEEL_CHAIR1=0] [WHEEL_CHAIR2=1] [ROAM=2] 1
        uint8_t res_type;     //预留 1 
    } sTYPE;
} unCHASSIS_CFG;

typedef union _FLOAT_ENV
{
    float   buff[3];
    struct
    {
      un_float encoder_k;     //编码器修正系数 (真实比当前编码器的速度)  4
      un_float joyReal_l_max; //摇杆真实角速度最大值(m/s) 4
			un_float joyReal_a_max; //摇杆真实角速度最大值(rad/s) 4
    } sF;
} unFLOAT_ENV;

typedef union __ENV_CONFIG
{
    uint32_t  u32_v[15];
    uint16_t  u16_v[30];
    uint8_t   u8_v[60];
    struct
    {
        unCHASSIS_CFG type;//底盘 电驱 电机 预留 4
        unFLOAT_ENV f;     //储存浮点环境变量         4*3=12
        IMU_FLASH_BUF imu; //imu 偏移量 4*5=20
        char ult[24];   //超声波通道使能12通道.预留12路,
        
    } sENV; // 20 +4*4 = 36 +24 =60 
} unENV_CFG;
//flash env 配置环境
extern unENV_CFG g_EnvCfg;


void initComFlashEnv(void);
uint32_t set_env_flash(unENV_CFG cfg);
void get_env_flash(unENV_CFG *cfg);  
bool get_float_SDkey(const char *dataName,float *value);

/*
判断x是否有限，是返回1，其它返回0；
int isfinite(x) 
判断x是否为一个数（非inf或nan），是返回1，其它返回0；
int isnormal(x)
当x时nan返回1，其它返回0；
int isnan(x)
当x是正无穷是返回1，当x是负无穷时返回-1，其它返回0。有些编译器不区分。
int isinf(x)
*/

#endif

