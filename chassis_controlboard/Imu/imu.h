#ifndef _IMU_H_
#define _IMU_H_

#include "spi_driver.h"
#include "spi.h"
#include "imu_maintain.h"
#pragma pack(1)


#define GYRO_CONVERSION  			(3.14159f/180.0f * 2000.0f/32768.0f)	//单位rad/s
#define ACC_CONVERSION   			(9.8f * 8.0f/32768.0f)	//单位m/t^2
#define FILTER_NUM   					500

#define sampleFreqDef   			100.0f          // sample frequency in Hz
#define betaDef         			0.01f            // 2 * proportional gain
#define UPDATAIMUTIME         0.005f

//imu错误
#define IMU_ERR_DRIFT    0x01		//漂移
#define IMU_ERR_NOISE    0x02		//噪声
#define IMU_ERR_ABNORM   0x04		//数据异常 未实现
#define IMU_ERR_HARDWARE 0x08	  //硬件错误
#define IMU_ERR_TEMP     0x10	  //温度异常

#define IMU_DETECT_DEPTH 20   //检测方差的缓存深度


typedef struct
{
	int16_t  	gyro_x;
	int16_t 	gyro_y;
	int16_t 	gyro_z;
	int16_t 	acc_x;
	int16_t 	acc_y;
	int16_t 	acc_z;
	int16_t 	temp;
}ImuData_TypeDef;

typedef struct 
{
	float Data;
    float Real_Gyro[3];
    float Real_ACC[3];
    float Angle_Z;
	  float temp;
    long int GyroDataSum[3];	//单位为度
    long int ACCDataSum[3];
    int StateFlag;
    int Gyro_Count;
    int Sum_Count;
    float Gyro_offset[3];
    float ACC_offset[3];
    float Gyro_FilterData[3];
    float lastGyro_FilterData[3][2];
    float ACC_FilterData[3];
    float orientation[4];
    unsigned char staticFlag;						//禁止状态，通过统计陀螺仪方差得到
    unsigned char vice_StaticFlag;				//副静止状态，由电机辅助做出判断
} ImuPara_TypeDef;


uint8_t initImuDevice(void);
uint8_t readImuDevice(void);
void getImuData(ImuPara_TypeDef *sensor);
uint8_t getImuState(void);
void clearImuState(void);

#endif


