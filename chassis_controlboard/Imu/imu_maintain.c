#include "imu_maintain.h"
#include "Clock.h"
#include "SD_Log_App.h"
#include "imu.h"
#include "math.h"
#include "idea.h"
#include "chassis_com.h"
#include "motor_driver.h"

#define OFFSET_P1 ((0.005341f)/180.0f*3.1415f)	//关于温度变化的斜率
//#define OFFSET_P1 (0.01f/180.0f*3.1415f)	//关于温度变化的斜率
#pragma pack(1)


uint8_t gyroCalibrationFlag = 0;


float imuRefTemp = 0;
float m_Ts = 0;
float m_GyroOffset[3];
MOTORCONTROL_Typedef *motor_inst_Ptr;


void read_imu_flash(float outData[4]);
void write_imu_flash(float *inData,uint8_t len);

static int detect_Static(float angleV);
static float getTs(void);
static int static_Delay(int static_in);
static float Drift_Queue(float *inGyro,float *outDrift);
static float Delay_Queue(float *inGyro,float *outGyro);

void write_imu_flash(float *inData,uint8_t len)
{
	IMU_FLASH_BUF save_buf;
	for(int i = 0;i < len;i++)
	{
		save_buf.f_data[i].data = inData[i];
	}	
	save_buf.c_data[16] = 0; //!这个地方请严格测试一遍
	
	if(EF_RW_ERROR == write_env_app("imu_offset", save_buf.c_data))
	{
		
	}
  //储存flash
  memcpy(g_EnvCfg.sENV.imu.c_data,save_buf.c_data,sizeof(IMU_FLASH_BUF));
	set_env_flash(g_EnvCfg);
}

void read_imu_flash(float outData[4])
{
	IMU_FLASH_BUF read_buf;
	char *str = NULL;
	str = read_env_app("imu_offset");
	if(str == NULL)
	{
    //从flash 读取
		for(int i = 0;i<4;i++)
		{
			outData[i] = g_EnvCfg.sENV.imu.f_data[i].data;
		}
		return;
	}

  //从 SD 读取参数
	for(int i = 0;i<16;i++)
	{
		read_buf.c_data[i] = str[i];
	}
  
	for(int i = 0;i<4;i++)
	{
		outData[i] = read_buf.f_data[i].data;
	}

	if(!isnormal(outData[0]+outData[1]+outData[2]+outData[3]))
	{
    outData[0] = 0;     
		outData[1] = 0;     
		outData[2] = 0;   
		outData[3] = 0;
  }
}



uint8_t Get_Gyro_Offset(float gyroOffset[4])
{
	
	//这个地方从flash中获取gyro的漂移
//	if(gyroCalibrationFlag == 0)
//	{
//		return 0;
//	}
	read_imu_flash(gyroOffset);	
	imuRefTemp = gyroOffset[3];
	motor_inst_Ptr = getMotorInst();
	return 0;
	
}
uint8_t Get_Gyro_Offset2(float gyroOffset[3])
{
	gyroOffset[0] = m_GyroOffset[0]/180.0f*3.1415f;
	gyroOffset[1] = m_GyroOffset[1]/180.0f*3.1415f;
	gyroOffset[2] = m_GyroOffset[2]/180.0f*3.1415f;
	
	return 0;
}

float gyro_CalDeltaOffset(float temp)
{
	if(!isnormal(imuRefTemp))
	{
		imuRefTemp = 0;
		return 0;
	}
	limitRange2(imuRefTemp,-20,80);//值域约束
	return (temp - imuRefTemp) * OFFSET_P1;
}

int Run_Gyro_Calibraton(int16_t *Gyro,float temp)
{
	uint8_t static selfState = 0;
	//uint8_t err = 0;
	static float gyroSum[4] = {0,0,0,0};
	static uint16_t times = 0;
	static float intoTime;
	if(gyroCalibrationFlag==0)
		return 0;	
	
	
	/*非阻塞的校准陀螺仪的状态机******************************/
	//err = 0x01;
	switch(selfState)		
	{
		case 0:
			intoTime = ( (float)getSystemTick() ) / 1000.0f;
		
			selfState = 1;
		break;
		
		case 1:
				//使用6s获取陀螺仪的漂移
				//返回成功或者失败
				//失败依据，校准过程中方差突变，需要使用高通滤波器，需要获取陀螺仪的噪声频谱
				//使用高通滤波器来获得噪声
			for(int i =0;i<3;i++)	
			{
				gyroSum[i] += Gyro[i];
			}
			times++;//作了多少次数据累加
			
			 if( (( (float)getSystemTick() ) / 1000.0f - intoTime) > GYRO_INIT_TIME )
			 {
				 selfState = 2;
			 }			
			break;
		case 2:
				//完成校准				
			for(int i =0;i<3;i++)	
			{
				gyroSum[i] /= times;
			}	
			times = 0;
			//存数据		
			gyroSum[3] = temp;
			write_imu_flash(gyroSum,4);
			
			imuRefTemp = temp;
			
			sdlog_printf_app("gyro reset over");
			
			gyroCalibrationFlag = 0;
			selfState = 0;
			break;
		default:
			selfState = 0;
			break;
	}
	/*end*非阻塞的校准陀螺仪的状态机******************************/		
	return 1;	
}

void Begin_Gyro_Calibration()
{
	gyroCalibrationFlag = 1;//激活陀螺仪校准
}
void Begin_Acc_Calibration()
{	
	//考虑是否暂时不需要做这个功能，因为对于姿态解算，它都是准确的
}

/*新的陀螺仪补偿算法
*内部单位使用°,°/s
*/
int Run_Gyro_Calibraton2(int16_t *Gyro,float temp)
{
//暂时只关心Z轴
	static float realAngleV[3];
	static int static_flag[3] = {0,0,0};
	realAngleV[0] = Gyro[0] * GYRO_CONVERSION/3.14159f*180.0f;
	realAngleV[1] = Gyro[1] * GYRO_CONVERSION/3.14159f*180.0f;
	realAngleV[2] = Gyro[2] * GYRO_CONVERSION/3.14159f*180.0f;
	getTs();
	static_flag[0] = detect_Static((realAngleV[0]+realAngleV[1]+realAngleV[2])/3.0f);
	static_flag[1] = static_Delay(static_flag[0]);	
	
	//降采样率
	static float sum_ts = 0;
	float signalBuf[3];									//用来传递数据
	float signalBuf2[3];									//用来传递数据
	sum_ts += m_Ts;
	if(sum_ts>0.1f) //10hz
	{
		sum_ts = 0;
		
		if(static_flag[1]==1)						//延时静置完成
		{
			Delay_Queue(realAngleV,signalBuf); //前置buf		
			static_flag[2] ++;						//延时等待前置buf填充完成
			if(static_flag[2]>3)					//等于buf的深度
			{	
				static float lk = 0.9;
				static_flag[2] = 3;						
				Drift_Queue(signalBuf,signalBuf2);
				m_GyroOffset[0] += lk * (signalBuf2[0] - m_GyroOffset[0]);		
				m_GyroOffset[1] += lk * (signalBuf2[1] - m_GyroOffset[1]);		
				m_GyroOffset[2] += lk * (signalBuf2[2] - m_GyroOffset[2]);

				lk += 0.01f*(0.02f - lk);  //按时间收敛滤波器作用，使滤波器开机时有更强的作用，而后几乎无作用
																		//更相信历史的数据，使瞬间的干扰降低
					
				limitRange2(m_GyroOffset[0],-3,3);//值域约束
				limitRange2(m_GyroOffset[1],-3,3);
				limitRange2(m_GyroOffset[2],-3,3);

				imuRefTemp += 0.1f * (temp - imuRefTemp);
				//limitRange2(imuRefTemp,-20,80);//值域约束 //在另外一个地方完成了这个事
			}
		}
		else
		{
			static_flag[2] = 0;
		}
						
	}	
	return static_flag[2];
}
float Drift_Queue(float *inGyro,float *outDrift)
{
	static float driftQueueBuf[5][3];
	float sum_buf[3] = {0,0,0};
	
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<3;j++)
		{
			driftQueueBuf[i][j] = driftQueueBuf[i+1][j];
			sum_buf[j] += driftQueueBuf[i+1][j];
		}		
	}
	driftQueueBuf[4][0] = inGyro[0];
	driftQueueBuf[4][1] = inGyro[1];
	driftQueueBuf[4][2] = inGyro[2];
	
	sum_buf[0] += inGyro[0];
	sum_buf[1] += inGyro[1];
	sum_buf[2] += inGyro[2];
	
	sum_buf[0] /= 5;
	sum_buf[1] /= 5;
	sum_buf[2] /= 5;
	
	outDrift[0] = sum_buf[0];
	outDrift[1] = sum_buf[1];
	outDrift[2] = sum_buf[2];

	return 0;
}
float Delay_Queue(float *inGyro,float *outGyro)
{
	static float delayQueueBuf[3][3];
	
	outGyro[0] = delayQueueBuf[0][0];
	outGyro[1] = delayQueueBuf[0][1];
	outGyro[2] = delayQueueBuf[0][2];
	
	for(int i=0;i<2;i++)
	{
		for(int j=0;j<3;j++)
		{
			delayQueueBuf[i][j] = delayQueueBuf[i+1][j];
		}		
	}
	delayQueueBuf[2][0] = inGyro[0];
	delayQueueBuf[2][1] = inGyro[1];
	delayQueueBuf[2][2] = inGyro[2];
	
	return 0;
}

int static_Delay(int static_in)
{
	static int counter_delay = 0;
	if(static_in == 1)
	{
		counter_delay++;
	}
	else
	{
		counter_delay = 0;
	}		
	
	if(counter_delay>100) //1s
	{
		counter_delay = 100;
		return 1;					//返回延时的静置条件
	}
	else
	{
		return 0;
	}
	
}

float getTs()
{
	static float lastStamp;
	float ts;
	float newStamp;
	newStamp = ( (float)getSystemTick() ) / 1000.0f;
	m_Ts = newStamp - lastStamp;
	lastStamp = newStamp;
	
	return ts;//返回最近的两次调用的时间间隔 (s)
}
float k22 = 22.6;
int detect_Static(float angleV)
{
	static float angleV_Last = 0;
	static float angleV_d[2];
	angleV_d[0] = (angleV - angleV_Last)/m_Ts;
	angleV_Last = angleV;
	angleV_d[1] += 0.5f*(angleV_d[0] - angleV_d[1]);
	
	static int counter = 20;
	
	//滤波器
	if((fabs(angleV_d[1])>k22)||!motor_inst_Ptr->lock)//大于9dps/s的角加速度
	{
		counter = 20;
	}
	else
	{
		counter--;
		if(counter<0)
			counter = 0;
	}
	
	if(counter==0)
	{
		return 1;		//返回静置状态
	}
	return 0;			//返回运动状态
}
