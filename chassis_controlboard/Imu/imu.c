#include "imu.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Clock.h"
#include "imu_driver.h"
#include "motor_driver.h"
#include "fault_injector_wrapper.h"
#include "alarm_com.h"
#include "SD_Log_App.h"

#define COMPENSATE_IMU						//注释它将不会有温度补偿和自动校准

float invSampleFreq = 0.01;
static uint8_t imu_state = 0x00;
//bit0	陀螺仪漂移过大
//bit1	陀螺仪噪声过大
//bit2	数据异常
//bit3	硬件错误
//bit4-bit7	预留

static ImuPara_TypeDef imu_sensor;


uint8_t initImuDevice(void)
{
    imu_state = 0x00;

    return initImuDriver();
}


void filterImuData(int16_t *Gyro, int16_t *ACC, ImuPara_TypeDef *pmsg)
{
    int i;
    static uint32_t imu_time = 0;
    static uint32_t lastimu_time = 0;
    static uint32_t imu_offsettime = 0;
	
		int isStatic = 0;										//不为0表明是静止的
    //陀螺仪漂移测量接入点
		Run_Gyro_Calibraton(Gyro,pmsg->temp);//校准值是重启生效的
		isStatic = Run_Gyro_Calibraton2(Gyro,pmsg->temp);		

    switch(pmsg->StateFlag)
    {
    case 0:
    {
        //这个地方作为初始化函数

        pmsg->Angle_Z = 0.0;
        pmsg->StateFlag = 1;
        for(i = 0; i < 3; i++)
        {
            pmsg->ACC_offset[i] = 0;
        }

				float offset_buf[4];
				Get_Gyro_Offset(offset_buf);
				pmsg->Gyro_offset[0] = offset_buf[0] * GYRO_CONVERSION;
				pmsg->Gyro_offset[1] = offset_buf[1] * GYRO_CONVERSION;
				pmsg->Gyro_offset[2] = offset_buf[2] * GYRO_CONVERSION;

				
        break;
    }
    case 1:
    {
			#ifdef COMPENSATE_IMU		//获取计算好的漂移
				float offset_buf[3];
				Get_Gyro_Offset2(offset_buf);
				pmsg->Gyro_offset[0] = offset_buf[0];
				pmsg->Gyro_offset[1] = offset_buf[1];
				pmsg->Gyro_offset[2] = offset_buf[2];
			#endif
        for(i=0;i<3;i++)		//把陀螺仪的数据做缓存以做平滑滤波
        {
            pmsg->lastGyro_FilterData[i][1] = pmsg->lastGyro_FilterData[i][0];
            pmsg->lastGyro_FilterData[i][0] = pmsg->Gyro_FilterData[i];
					
            //pmsg->Gyro_FilterData[i] = (float)(Gyro[i] - pmsg->Gyro_offset[i]);
						pmsg->Gyro_FilterData[i] = (float)Gyro[i];
            //pmsg->ACC_FilterData[i] = (float)(ACC[i] - pmsg->ACC_offset[i]);
            pmsg->ACC_FilterData[i] = (float)ACC[i];
        }

        for(i=0; i<3; i++)
        {
						//平滑滤波
            pmsg->Gyro_FilterData[i] = (pmsg->lastGyro_FilterData[i][1] + pmsg->lastGyro_FilterData[i][0] + pmsg->Gyro_FilterData[i])/3;
						pmsg->Real_Gyro[i] = pmsg->Gyro_FilterData[i] * GYRO_CONVERSION - pmsg->Gyro_offset[i];						
						#ifdef COMPENSATE_IMU
							pmsg->Real_Gyro[2] -= gyro_CalDeltaOffset(pmsg->temp);//去掉由温度变化带来的漂移
						#endif			
            pmsg->Real_ACC[i]  = pmsg->ACC_FilterData[i] * ACC_CONVERSION;
        }

				if((isStatic!=0)&&(fabs(pmsg->Real_Gyro[2])<0.005f))					//如果静止，陀螺仪置0
        {                
					pmsg->Real_Gyro[0] = 0;
					pmsg->Real_Gyro[1] = 0;
					pmsg->Real_Gyro[2] = 0;
        }
				
        lastimu_time = imu_time;
        imu_time = getSystemTick();	//millis();
        imu_offsettime = imu_time - lastimu_time;
        pmsg->Data = (pmsg->Real_Gyro[2]) * imu_offsettime/1000; //ms to s  //+0.0930

        pmsg->Angle_Z += pmsg->Data/3.14159f*180.0f;

        break;
    }
    }
}

float calImuInvSqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;

    i = 0x5f3759df - (i>>1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));

    return y;
}

void calculateQuaternionData(float gx, float gy, float gz, float ax, float ay, float az, ImuPara_TypeDef *pmsg)
{
    float recipNorm;
    float s0, s1, s2, s3;
    float qDot1, qDot2, qDot3, qDot4;
    float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

    float beta = betaDef;
    static float q0 = 1.0f;
    static float q1 = 0.0f;
    static float q2 = 0.0f;
    static float q3 = 0.0f;	// quaternion of sensor frame relative to auxiliary frame
    //float invSampleFreq = 1.0f / sampleFreqDef;

    // Convert gyroscope degrees/sec to radians/sec
    //    gx *= 0.0174533f;//参数传递的是rad/s不再需要换算
    //    gy *= 0.0174533f;
    //    gz *= 0.0174533f;

    // Rate of change of quaternion from gyroscope
    qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz);
    qDot2 = 0.5f * (q0 * gx + q2 * gz - q3 * gy);
    qDot3 = 0.5f * (q0 * gy - q1 * gz + q3 * gx);
    qDot4 = 0.5f * (q0 * gz + q1 * gy - q2 * gx);

    // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
    if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
    {
        // Normalise accelerometer measurement
        recipNorm =  calImuInvSqrt(ax * ax + ay * ay + az * az);
        ax *= recipNorm;
        ay *= recipNorm;
        az *= recipNorm;

        // Auxiliary variables to avoid repeated arithmetic
        _2q0 = 2.0f * q0;
        _2q1 = 2.0f * q1;
        _2q2 = 2.0f * q2;
        _2q3 = 2.0f * q3;

        _4q0 = 4.0f * q0;
        _4q1 = 4.0f * q1;
        _4q2 = 4.0f * q2;

        _8q1 = 8.0f * q1;
        _8q2 = 8.0f * q2;

        q0q0 = q0 * q0;
        q1q1 = q1 * q1;
        q2q2 = q2 * q2;
        q3q3 = q3 * q3;

        // Gradient decent algorithm corrective step
        s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
        s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
        s2 = 4.0f * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
        s3 = 4.0f * q1q1 * q3 - _2q1 * ax + 4.0f * q2q2 * q3 - _2q2 * ay;
        recipNorm =  calImuInvSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
        s0 *= recipNorm;
        s1 *= recipNorm;
        s2 *= recipNorm;
        s3 *= recipNorm;

        // Apply feedback step
        qDot1 -= beta * s0;
        qDot2 -= beta * s1;
        qDot3 -= beta * s2;
        qDot4 -= beta * s3;
    }

    // Integrate rate of change of quaternion to yield quaternion
    q0 += qDot1 * invSampleFreq;
    q1 += qDot2 * invSampleFreq;
    q2 += qDot3 * invSampleFreq;
    q3 += qDot4 * invSampleFreq;

    // Normalise quaternion
    recipNorm =  calImuInvSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
    q0 *= recipNorm;
    q1 *= recipNorm;
    q2 *= recipNorm;
    q3 *= recipNorm;

    pmsg->orientation[0] = q0;
    pmsg->orientation[1] = q1;
    pmsg->orientation[2] = q2;
    pmsg->orientation[3] = q3;
    //	anglesComputed = 0;
}

void readQuaternionData(ImuPara_TypeDef *pmsg)
{
    double Imusecs;
    static double last_Imusecs;
    //float invSampleFreq = 1.0f / sampleFreqDef;

    Imusecs = getSystemTick();	//millis();

    if(pmsg->StateFlag == 1 && (Imusecs - last_Imusecs >= UPDATAIMUTIME * 1000))
    {
        invSampleFreq = (float)((Imusecs - last_Imusecs)/1000); //Time difference
        calculateQuaternionData(pmsg->Real_Gyro[0], pmsg->Real_Gyro[1], pmsg->Real_Gyro[2],
                                pmsg->Real_ACC[0],  pmsg->Real_ACC[1],  pmsg->Real_ACC[2], pmsg);

        last_Imusecs = Imusecs;
    }

}
//=============================================================================================================//


#pragma pack(1)

typedef struct
{
    float data[3];
} vector3_TypeDef;


//统计方差和期望 0.2s 20个数据 输出IMU错误
//连续测三次
static unsigned int detecter[2] = {0,0};//检测到漂移，检测到噪声。（缓存次数）
static vector3_TypeDef gyroExpectation;

uint8_t detectImuData(ImuPara_TypeDef *pmsg)
{
    static unsigned int count = 0;
    static vector3_TypeDef gyroBuf[IMU_DETECT_DEPTH];

    static float gyroExpectationSum;
    static vector3_TypeDef gyroViranceBuf;
    static float gyroVirance;
    static unsigned int err;
		static bool b_logGyro = false ,b_logNoise = false;

    count++;
    if(count>IMU_DETECT_DEPTH-1)
    {
        count = 0;
        gyroExpectation.data[0] /= IMU_DETECT_DEPTH;
        gyroExpectation.data[1] /= IMU_DETECT_DEPTH;
        gyroExpectation.data[2] /= IMU_DETECT_DEPTH;

        for(int n=0; n<IMU_DETECT_DEPTH; n++)
        {
            gyroViranceBuf.data[0] += fabs(gyroBuf[n].data[0] - gyroExpectation.data[0]);
            gyroViranceBuf.data[1] += fabs(gyroBuf[n].data[1] - gyroExpectation.data[1]);
            gyroViranceBuf.data[2] += fabs(gyroBuf[n].data[2] - gyroExpectation.data[2]);
        }
        gyroViranceBuf.data[0] /= IMU_DETECT_DEPTH;
        gyroViranceBuf.data[1] /= IMU_DETECT_DEPTH;
        gyroViranceBuf.data[2] /= IMU_DETECT_DEPTH;
        //获得方差
        gyroVirance = gyroViranceBuf.data[0] + gyroViranceBuf.data[1] + gyroViranceBuf.data[2];

        //期望的几何距离
        gyroExpectationSum = sqrt(gyroExpectation.data[0]*gyroExpectation.data[0]\
                                  +gyroExpectation.data[1]*gyroExpectation.data[1]\
                                  +gyroExpectation.data[2]*gyroExpectation.data[2]);
        //漂移检测
        if(gyroVirance<0.05f)
        {
            pmsg->staticFlag = 1;
						#ifdef __TP_USER
						TP_MODIFY_VALUE(eTP33_imu_err_drift,gyroExpectationSum,0.06);
						#endif		
            if(gyroExpectationSum > 0.05f)
            {
                detecter[0]++;
                if(detecter[0]>15)
                {
                    err |= IMU_ERR_DRIFT;
										setAlarm(eA_INTE,eb00_imuGyro_OverDrift_Err,ALARM_ERR);
										if(!b_logGyro)
										{
											GR_LOG_ERROR("the gyro may not be reset");
											b_logGyro = true;
										}
                    detecter[0] = 100;
                }
//								else
//								{
//									if(b_logGyro == true)
//									{
//										GR_LOG_WARN("the gyro  Already reset recover");
//									}
//									b_logGyro = false;
//									setAlarm(eA_INTE,eb00_imuGyro_OverDrift_Err,ALARM_OK);
//                  err &= ~IMU_ERR_DRIFT;
//								}
            } 
						else
            {
                detecter[0] = 0;
								if(b_logGyro == true)
								{
									GR_LOG_WARN("the gyro  Already reset recover");
								}
								b_logGyro = false;
								setAlarm(eA_INTE,eb00_imuGyro_OverDrift_Err,ALARM_OK);
                err &= ~IMU_ERR_DRIFT;
            }
        } 
				else
        {
            detecter[0] = 0;
            pmsg->staticFlag = 0;
        }
        //漂移检测end

        //噪声检测
				#ifdef __TP_USER
				TP_MODIFY_VALUE(eTP34_imu_err_noise,gyroVirance,16);
				#endif
        if(pmsg->vice_StaticFlag)//5Hz
        {
            if(gyroVirance>15)//方差基本不会大于15
            {
                detecter[1]++;
                if(detecter[1]>15)//3s
                {
                    err |= IMU_ERR_NOISE; //置位
										if(!b_logNoise)
										{
											GR_LOG_ERROR("alarm imu noise error");
											b_logNoise = true;
										}
										setAlarm(eA_INTE,eb01_imuGyro_OverNoise_Err,ALARM_ERR);
                    detecter[1] = 100;
                }
            } 
						else
            {
								if(b_logNoise == true)
								{
									GR_LOG_WARN("alarm imu noise error recover ");
									b_logNoise = false;
								}
								setAlarm(eA_INTE,eb01_imuGyro_OverNoise_Err,ALARM_OK);
                detecter[1] = 0;
                err &= ~IMU_ERR_NOISE;//清除位
            }
        }
        //噪声检测end

        gyroExpectation.data[0] = 0;
        gyroExpectation.data[1] = 0;
        gyroExpectation.data[2] = 0;
    }
    gyroBuf[count].data[0] = pmsg->Real_Gyro[0]/3.1415f*180.0f;
    gyroBuf[count].data[1] = pmsg->Real_Gyro[1]/3.1415f*180.0f;
    gyroBuf[count].data[2] = pmsg->Real_Gyro[2]/3.1415f*180.0f;

    gyroExpectation.data[0] += gyroBuf[count].data[0];
    gyroExpectation.data[1] += gyroBuf[count].data[1];
    gyroExpectation.data[2] += gyroBuf[count].data[2];
    return err;
}
//=============================================================================================================//

uint8_t readImuData(ImuPara_TypeDef *pmsg)
{
    static ImuData_TypeDef MPU9250_Data;
    uint8_t err;
		static bool b_logComHwd = false,b_logImuTemp = false;
		
    memset(&MPU9250_Data, 0x00, sizeof(ImuData_TypeDef));
    err = readImuRawData(&MPU9250_Data);

		#ifdef __TP_USER
						TP_MODIFY_VALUE(eTP32_imu_err_hardware,err,IMU_ERROR);
		#endif		
    if(err != IMU_OK)
    {
				setAlarm(eA_INTE,eb03_imuCommunHdwFatal,ALARM_ERR);
				if (!b_logComHwd)
				{
					GR_LOG_ERROR("alarm imu imuCommunHdwFatal error");
					b_logComHwd = true;
				}
        return IMU_ERR_HARDWARE;
    }

    if((MPU9250_Data.gyro_x == 0) &&
            (MPU9250_Data.gyro_y == 0) &&
            (MPU9250_Data.gyro_z == 0) &&
            (MPU9250_Data.acc_x  == 0) &&
            (MPU9250_Data.acc_y  == 0) &&
            (MPU9250_Data.acc_z  == 0))
    {
				setAlarm(eA_INTE,eb03_imuCommunHdwFatal,ALARM_ERR);
				if (!b_logComHwd)
				{
					GR_LOG_ERROR("alarm imu imuCommunHdwFatal error");
					b_logComHwd = true;
				}
        return IMU_ERR_HARDWARE;
    }

    filterImuData(&MPU9250_Data.gyro_x, &MPU9250_Data.acc_x, pmsg);

    readQuaternionData(pmsg);
    pmsg->temp = MPU9250_Data.temp/326.8f + 25.0f;

    #ifdef __TP_USER
    TP_MODIFY_VALUE(eTP55_imu_temp_hight_err,pmsg->temp, (g_cfg.sA.inte.cfg.b04_imuTempErr_v+1 ));
		#endif	
    if(pmsg->temp > g_cfg.sA.inte.cfg.b04_imuTempErr_v)
    {
      if(!b_logImuTemp)
      {
         b_logImuTemp = true;
         GR_LOG_ERROR(" imu temp error [%f]",pmsg->temp);
         setAlarm(eA_INTE,eb04_imuTempErr,ALARM_ERR);
      }
			err |= IMU_ERR_TEMP;						//温度过高	
    }
    else
    {
      if(b_logImuTemp)
      {
        b_logImuTemp = false;
        setAlarm(eA_INTE,eb04_imuTempErr,ALARM_OK);
        GR_LOG_WARN(" imu temp recover [%f]",pmsg->temp);
      }
			err &= ~IMU_ERR_TEMP;
    }

    //硬件错误 依据，返回的所有IMU的数据都是0xffff(-1)
    static unsigned char err_Count;
    if((MPU9250_Data.gyro_x == -1) &&
            (MPU9250_Data.gyro_y == -1) &&
            (MPU9250_Data.gyro_z == -1) &&
            (MPU9250_Data.acc_x  == -1) &&
            (MPU9250_Data.acc_y  == -1) &&
            (MPU9250_Data.acc_z  == -1))
    {
        err_Count++;
        if(err_Count>10)//连续10次判断为错误
				{
					err |= IMU_ERR_HARDWARE;						//硬件错误					
					setAlarm(eA_INTE,eb03_imuCommunHdwFatal,ALARM_ERR);
					if (!b_logComHwd)
					{
						GR_LOG_ERROR("alarm imu CommunHdwFatal error");
						b_logComHwd = true;
					}
				}
    }
    else
    {
        err_Count = 0;
    }
				
    err |= detectImuData(pmsg);//检测陀螺仪漂移和噪声
		setAlarm(eA_INTE,eb03_imuCommunHdwFatal,ALARM_OK);
		if (b_logComHwd == true)
		{
			GR_LOG_WARN("alarm imu CommunHdwFatal error revoer");
			b_logComHwd = false;
		}
		
    return err;
}


uint8_t readImuDevice(void)
{
    MOTORCONTROL_Typedef motor_data;

    updateMotorData(&motor_data);
    imu_sensor.vice_StaticFlag = motor_data.lock;
    imu_state = readImuData(&imu_sensor);

    return imu_state;
}

void getImuData(ImuPara_TypeDef *sensor)
{
    memcpy(sensor, &imu_sensor, sizeof(ImuPara_TypeDef));
}

uint8_t getImuState(void)
{
    return imu_state;
}

void clearImuState(void)
{
    imu_state = 0;
    detecter[0] = 0;
    detecter[1] = 0;
    gyroExpectation.data[0] = 0;
    gyroExpectation.data[1] = 0;
    gyroExpectation.data[2] = 0;
}
