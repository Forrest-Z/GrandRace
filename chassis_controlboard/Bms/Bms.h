#ifndef __BMS_H__
#define __BMS_H__
#include "stm32f4xx_hal.h"
#include "OD.h"
#include "Publish_powerboard.h"

/************************************************
 * 定义带位域的联合体类型，用作系统所有的标志位处理
 ************************************************/
#define BMS_DISCONNECT 						(0x01 << 5)
//三次之后再上告警
#define BMS_LOG_MAX_CNT 					  2
#define BMS_LOG_OUT 					      3

typedef union
{
    uint32_t  u32_v;
    uint16_t  u16_v[2];
    uint8_t   u8_v[4];
    struct
    {
        __IO uint8_t b00_low_power: 1;   			    //低电量
        __IO uint8_t b01_low_temp: 1; 						//低温
        __IO uint8_t b02_high_temp: 1; 					  //高温
        __IO uint8_t b03_high_current: 1;  			  //高电流
        __IO uint8_t b04_low_voltage: 1; 				  //低电压
        __IO uint8_t b05_dis_connection: 1; 			//连接断开
        __IO uint8_t b06_get_data_faild: 1; 			//获取BMS数据失败
        __IO uint8_t b07_ultra_low_power: 1; 		  //超低电量
        __IO uint8_t b08_ultra_low_temp: 1; 			//超低温
        __IO uint8_t b09_ultra_high_temp: 1; 	 	  //超高温
        __IO uint8_t b10_ultra_high_current: 1;	  //超高电流
        __IO uint8_t b11_ultra_low_voltage: 1;    //超低电压
        __IO uint8_t b12_12v_int_power_v: 1;   	  //12V内部电压异常
        __IO uint8_t b13_tx2_power_v: 1;   			  //TX2电压异常
        __IO uint8_t b14_5v_power_v: 1;     			//5V电压异常
        __IO uint8_t b15_12v_out_power_v: 1;    	//12V外部电压异常
        __IO uint8_t b16_pad_power_i: 1;					//PAD供电过流
        __IO uint8_t b17_out_i: 1;    						//外设供电过流(超声波/路由器/RTK)
        __IO uint8_t b18_joys_over_i: 1;   			  //摇杆供电过流
        uint8_t b19: 1;     //预留
        uint8_t b20: 1;
        uint8_t b21: 1;
        uint8_t b22: 1;
        uint8_t b23: 1;
        uint8_t b24: 1;
        uint8_t b25: 1;
        uint8_t b26: 1;
        uint8_t b27: 1;
        uint8_t b28: 1;
        uint8_t b29: 1;
        uint8_t b30: 1;
        uint8_t b31: 1;
    } flag;
} eBMS;    //sizeof(eBMS) 为 4



#define WARN_TH_TEMPER_ULTRALOW						-20			// �C
#define WARN_TH_TEMPER_LOW								-10			// �C
#define WARN_TH_TEMPER_HIGH								80			// �C
#define WARN_TH_TEMPER_ULTRAHIGH					85			// �C
#define WARN_TH_CURRENT_HIGH						  40			// A
#define WARN_TH_CURRENT_EXCESS					  55			// A
#define WARN_TH_VOLTAGE_LOW						  	39			// V
#define WARN_TH_VOLTAGE_ULTRALOW					38			// V
#define WARN_TH_CAPACITY_LOW						  20			// % 低电量
#define WARN_TH_CAPACITY_ULTRALOW					10			// % 超低电量


typedef struct {
    uint8 bPowerComFlag;
    uint8 bV_5vErrorFlag;
    uint8 bV_In12vErrFlag;
    uint8 bV_Out12vErrFlag;
    uint8 bV_Tx2ErrFlag;
    uint8 bJoyCurrentStatus;
    uint8 bPadCurStatus;
    uint8 bRtkCurSatus;
    uint8 bBmsDisc;
    uint8 bBmsTemp;
    uint8 bBmsI;
    uint8 bBmsV;
    uint8 bBmsSoc;
    uint8 bPowerT;//Powerboard Temp
} bBMS_LOGFLAG;

void checkBmsData(void);
uint32_t getBmsState(void);
void clearBmsState(void);
float getBmsTotalVolt(void); //获得电池电压

#endif


