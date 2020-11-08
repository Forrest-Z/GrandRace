#include "imu_driver.h"
#include "Clock.h"
#include "Motor_PcConfig.h"

IMU_PARAMETER imu_parameter = //IMU安装参数
{
    .GYRO_X_ADJUST_ = 1,
    .GYRO_Y_ADJUST_ = 0,
    .GYRO_Z_ADJUST_ = 2,
    .GYRO_X_NEG_    = 1,
    .GYRO_Y_NEG_    = -1,
    .GYRO_Z_NEG_    = 1,
    .ACC_X_ADJUST_  = 1,
    .ACC_Y_ADJUST_  = 0,
    .ACC_Z_ADJUST_  = 2,
    .ACC_X_NEG_     = 1,
    .ACC_Y_NEG_     = -1,
    .ACC_Z_NEG_     = 1
};




// MPU9250 设置低通滤波器
uint8_t setImuLPF(uint8_t lpf)
{
    uint8_t dat = 0;

    if     (lpf > 184) dat = 1;
    else if(lpf > 92)  dat = 2;
    else if(lpf > 41)  dat = 3;
    else if(lpf > 20)  dat = 4;
    else if(lpf > 10)  dat = 5;
    else 						   dat = 6;

    return writeSpiSingleByte(MPU_CFG_REG, dat);
}

// MPU9250 设置采样频率
uint8_t setImuRate(uint16_t rate)
{
    uint8_t dat = 0;
    HAL_StatusTypeDef errorcode = HAL_OK;

    if(rate > 1000)
        rate = 1000;
    if(rate < 4)
        rate = 4;

    dat = 1000/rate - 1;

    errorcode = writeSpiSingleByte(MPU_SAMPLE_RATE_REG, dat);
    if(errorcode != HAL_OK)
        return errorcode;

    return setImuLPF(rate/2);      // 自动设置数字低通滤波为采样频率的一半
}

// 设置陀螺仪
uint8_t setImuGyro(uint8_t fsr)						//0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
{
    return writeSpiSingleByte(MPU_GYRO_CFG_REG, fsr<<3);
}

// 设置加速度计范围
uint8_t setImuAccel(uint8_t fsr) //0,±2g;1,±4g;2,±8g;3,±16g
{
    return writeSpiSingleByte(MPU_ACCEL_CFG_REG, fsr<<3);
}

// =========================================================================================================================

// 初始化IMU设备驱动
uint8_t initImuDriver(void)
{
    uint8_t get_ID = 0;

    writeSpiSingleByte(MPU_PWR_MGMT1_REG, 0x80);//复位MPU9250
    delayUs(100000);			//Delay_ms(100);
    writeSpiSingleByte(MPU_PWR_MGMT1_REG, 0x03);//唤醒MPU9250 00
    setImuGyro(3);				// 陀螺仪传感器,±250dps;1,±500dps;2,±1000dps;3,±2000dps
    setImuAccel(2);			// 加速度传感器,±2g->0 ±4->1 ±8->2
    setImuRate(200);			// 设置采样频率
    writeSpiSingleByte(MPU_INT_EN_REG, 0x00);		// 关闭所有中断
    writeSpiSingleByte(MPU_USER_CTRL_REG, 0x00); // I2C主模式关闭，HCM588L由主总线驱动
    writeSpiSingleByte(MPU_FIFO_EN_REG, 0x00);	  // 关闭FIFO
    writeSpiSingleByte(MPU_INTBP_CFG_REG, 0x80); // INT引脚低电平有效
    readSpiSingleByte(MPU_DEVICE_ID_REG, &get_ID);

    //if(get_ID == 0x71)   	// MPU9250
    if(get_ID == 0x12)			// ICM20602
    {
        writeSpiSingleByte(MPU_PWR_MGMT1_REG, 0x03);		//设置CLKSEL,PLL X轴为参考
        writeSpiSingleByte(MPU_PWR_MGMT2_REG, 0x00);		//加速度与陀螺仪都工作
        setImuRate(200);												//设置采样频率

        return IMU_OK;
    }
    else
    {
        return IMU_ERROR;
    }
}

uint8_t readImuRawData(ImuData_TypeDef *pmsg)
{
    static uint8_t read_buf[14] = {0x00};
    uint8_t err;

    if(pmsg == NULL)
        return IMU_ERROR;

    err = readSpiMultiBytes(ACCEL_XOUT_H, read_buf, 14);
    if(err != IMU_OK)
        return err;

    int16_t gyro_buf[3];
    int16_t acc_buf[3];
	  int16_t temp_buf;
    gyro_buf[0] = (int16_t)((read_buf[8] << 8) + read_buf[9]);
    gyro_buf[1] = (int16_t)((read_buf[10] << 8) + read_buf[11]);
    gyro_buf[2] = (int16_t)((read_buf[12] << 8) + read_buf[13]);

    acc_buf[0] = (int16_t)((read_buf[0] << 8) + read_buf[1]);
    acc_buf[1] = (int16_t)((read_buf[2] << 8) + read_buf[3]);
    acc_buf[2] = (int16_t)((read_buf[4] << 8) + read_buf[5]);
	  temp_buf = (int16_t)((read_buf[6] << 8) + read_buf[7]);

    // 数据异常保护

    pmsg->gyro_x = imu_parameter.GYRO_X_NEG_ * gyro_buf[imu_parameter.GYRO_X_ADJUST_];
    pmsg->gyro_y = imu_parameter.GYRO_Y_NEG_ * gyro_buf[imu_parameter.GYRO_Y_ADJUST_];
    pmsg->gyro_z = imu_parameter.GYRO_Z_NEG_ * gyro_buf[imu_parameter.GYRO_Z_ADJUST_];

    pmsg->acc_x = imu_parameter.ACC_X_NEG_ * acc_buf[imu_parameter.ACC_X_ADJUST_];
    pmsg->acc_y = imu_parameter.ACC_Y_NEG_ * acc_buf[imu_parameter.ACC_Y_ADJUST_];
    pmsg->acc_z = imu_parameter.ACC_Z_NEG_ * acc_buf[imu_parameter.ACC_Z_ADJUST_];

	pmsg->temp = temp_buf;
    return IMU_OK;
}

//IMU_PARAMETER *getImuParaPtr(void)
//{
//	return &imu_parameter;
//}

void configImuPara(IMU_PARAMETER *para)
{
    memcpy(&imu_parameter, para, sizeof(IMU_PARAMETER));
    // 数据异常保护
}

