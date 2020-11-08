#ifndef __NEW_MOTOR_CONTROL_H
#define __NEW_MOTOR_CONTROL_H
#include	"Mb_Master.h"
#include "Mb_crc16.h"
#include "usart.h"
#include "motor_driver_config.h"
#include "control_config.h"
/**************Motor Parameter*********************/
typedef struct {
    uint16_t RatedCurrent;
    uint16_t MaxLoadCurrent;
    uint16_t MotorStudy;
    uint16_t SpeedUp_Buffer_Time;
    uint16_t SpeedDown_Buffer_Time;
    uint16_t Max_UpAcceleration;
    uint16_t Default_UpAcceleration;
    uint16_t Max_DownAcceleration;
    uint16_t Default_DownAcceleration;
    uint16_t Max_Speed;
    uint16_t Default_Speed;
    uint16_t Speed_Closed_Loop;
    uint16_t Location_Closed_Loop;
} MOTOR_PARAMETER;

/*************Parameter Type**********************/
typedef enum {
    SpeedUp_Buffer_Time_Type = 1,
    SpeedDown_Buffer_Time_Type,
    Max_UpAcceleration_Type,
    Default_UpAcceleration_Type,
    Max_DownAcceleration_Type,
    Default_DownAcceleration_Type,
    Max_Speed_Type,
    Default_Speed_Type,
    All_Type
} PARAMETER_TYPR;

/*************Motor Type**********************/
#define Left_Motor      0
#define Right_Motor     1
#define LeftRight_Motor 2


/*************Pid Parameter**********************/
#define kp 0.35f
#define ki 0.002f
#define kd 0.001f

/*************Motor Driver Address**********************/
#define Left_Motor_Address  0x01
#define Right_Motor_Address 0x02


typedef struct {
    float Kp;
    float Ki;
    float Kd;
} SET_PID_STRUCY;
typedef struct {
    float Kp;
    float Ki;
    float Kd;
} READ_PID_STRUCY;
typedef struct {
    int16_t LeftMotor_frequency;
    int16_t RightMotor_frequency;
} MOTOR_Frequency;
typedef struct {
    float LeftMotor_RealSpeed;
    float RightMotor_RealSpeed;
} MOTOR_REALSPEED;
typedef struct {
    uint16_t LeftMotor_Status;
    uint16_t RightMotor_Status;
} MOTOR_STATUS;
typedef struct {
    int     RightMotor_Toale_Pulse_Number;
    int     LeftMotor_Toale_Pulse_Number;
} MOTOR_TOALE_Pulse_Number;
typedef struct {
    float     RightMotor_Toale_Trip;
    float     LeftMotor_Toale_Trip;
} MOTOR_TOALE_TRIP;
typedef struct {
    uint16_t LeftMotor_StudyStatus;
    uint16_t RightMotor_StudyStatus;
} MOTORSTUDY_STATUS;
typedef struct {
    uint16_t LeftMotor_StudyStatus;
    uint16_t RightMotor_StudyStatus;
} MOTORWORK_STATUS;
typedef struct {
    uint16_t LeftMotor_WorkType;
    uint16_t RightMotor_WorkType;
} MOTORWORK_TYPE;

//BRK 1刹车 0不刹车
#define AQMDBL_EN_Lock(x) x?HAL_GPIO_WritePin(BREAK_GPIO_Port,BREAK_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(BREAK_GPIO_Port,BREAK_Pin,GPIO_PIN_RESET)

void Init_NewMotor_Communication(void);
void Set_LeftRight_Motor_RatedCurrent(void);
void Set_LeftRight_Motor_MaxLoadCurrent(void);
void LeftRightMotor_Study(void);
void Set_LeftRightMotor_Parameter(void);
void Set_Motor_Speed(float speed,uint8_t motor_type);
void Set_Motor_Stop(uint16_t Stop_Type,uint8_t motor_type);
void Set_Motor_ControlType(void);
void Set_Motor_PIDParameter(void);
//void Read_MotorRealSpeed(MOTOR_REALSPEED *const speed);
void Read_MotorRealSpeed(void);
void Read_PID_Parameter(void);
void Read_MotorStatus(void);
void Read_Total_Trip(void);
void Read_MotorStudy_Status(void);
void Read_MotorWorkType(void);
void AQMDBL_scramFlag(unsigned char *pFlag);		//wqw 用于检测驱动器是否供电
#endif
