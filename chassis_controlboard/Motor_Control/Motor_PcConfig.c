#include "Motor_PcConfig.h"
#include "control_config.h"
#include "idea.h"
#include "chassis_com.h"
#include "SD_Log_APP.H"

MOTOR_PARAMETER_ motor_parameter;
CHASSIS_PARAMETER_ chassis_parameter;
DRIVER_PARAMETER_ driver_parameter;

/*************************************************
Function:       readMotorFlash
Description:    从flash读取 motor 参数
return :        chassis 
*************************************************/
uint8_t readMotorFlash(void)
{
    uint8_t motor = 0;
    
    if((!isnormal(g_EnvCfg.sENV.type.sTYPE.motor_type))
            || (g_EnvCfg.sENV.type.sTYPE.motor_type > MOTOR_MAX ))
    {
        motor = MOTOR_TYPE_DEFAULT;
    }
    else
    {
        //MOTOR_TYPE
        motor = g_EnvCfg.sENV.type.sTYPE.motor_type;
    }
    return motor;
}

void defaultMotorPara(void)
{
    switch(motor_parameter.motor_type)
    {
      case MOTOR_ORIGINAL:
      {
          motor_parameter.diameter_wheel 				= 0.244f;
          motor_parameter.scale_wheel    				= (91*3);
          motor_parameter.Number_of_motor_pole_ = 8.0f;
          motor_parameter.Reduction_ratio_      = 23.0f;
          motor_parameter.k = 0.6815f;
          motor_parameter.b = 0.0;
          motor_parameter.Encoder_K = 1.0f;
          break;
      }
      case MOTOR_ZHAOYANG:
      {
          motor_parameter.diameter_wheel 				= 0.304f;
          motor_parameter.scale_wheel    				= (44*3);
          motor_parameter.Number_of_motor_pole_ = 20.0f;
          motor_parameter.Reduction_ratio_      = 4.4f;
          motor_parameter.k = 0.22f;													//有机会需要再测一次
          motor_parameter.b = 0.0;
          motor_parameter.Encoder_K = 4.505f;
          break;
      }
      case MOTOR_JIQIN90N:
      {
          motor_parameter.diameter_wheel 				= 0.304f;
          motor_parameter.scale_wheel    				= (90*3);
          motor_parameter.Number_of_motor_pole_ = 20.0f;
          motor_parameter.Reduction_ratio_      = 9.0f;
          motor_parameter.k = 0.2913f;													//有机会需要再测一次
          motor_parameter.b = 0.0;
          motor_parameter.Encoder_K = 2.2491;
          break;
      }
      case MOTOR_LIANYI60N:
      {
          motor_parameter.diameter_wheel 				= 0.304f;
          motor_parameter.scale_wheel    				= (75.0f*3.0f);
          motor_parameter.Number_of_motor_pole_ = 10.0f;
          motor_parameter.Reduction_ratio_      = 15.0f;
          motor_parameter.k = 0.3639f;
          motor_parameter.b = 0.0;
          motor_parameter.Encoder_K = 1.261f;
          break;
      }
      case MOTOR_LIANYI_NEW:
      {
          motor_parameter.diameter_wheel 				= 0.304f;
          motor_parameter.scale_wheel    				= (75.0f*3.0f);
          motor_parameter.Number_of_motor_pole_ = 10.0f;
          motor_parameter.Reduction_ratio_      = 15.0f;
          motor_parameter.k = 0.3427f; //电压（归一化）比转速
          motor_parameter.b = 0.0;
          motor_parameter.Encoder_K = 1.313458f;
          break;
      }
    }
}


MOTOR_PARAMETER_ *Read_MotorType(void)
{
    char *str;
    uint8_t motortype;
    str = read_env_app("motor");

    if(str == NULL)
    {
        motortype = readMotorFlash();
        GR_LOG_INFO("flash motor read = %d",motortype);
    }
    else
    {
        motortype = atoi(str);
        if(motortype > MOTOR_MAX)
        {
            motortype = readMotorFlash();
            GR_LOG_INFO("have sd,but read flash motor = %d",motortype);
        }
        else
        {
            GR_LOG_INFO("SD motor read = %d",motortype);
        }
    }
    motor_parameter.motor_type = motortype;
    
    defaultMotorPara();

    upDataEncoder_K(&motor_parameter.Encoder_K);
    return &motor_parameter;
}
/*************************************************
Function:       upDataEncoder_K
Description:    更新编码值
Input: pEncoder_K : 更新数值
*************************************************/
void upDataEncoder_K(float *pEncoder_K)
{
    float buf,last_value;

    if (false == get_float_SDkey("reset_odom",&buf))
    {
        // 如果一旦SD 读取失败,使用初始化标定默认值
        GR_LOG_WARN("encoder scale is defult is: %f",*pEncoder_K); 
        if(g_EnvCfg.sENV.f.sF.encoder_k.data != *pEncoder_K)
        {
          last_value = g_EnvCfg.sENV.f.sF.encoder_k.data;
          g_EnvCfg.sENV.f.sF.encoder_k.data = *pEncoder_K;
          if(set_env_flash(g_EnvCfg) == OK)
          {
            sdlog_printf_app("flash set defult reset_odom ok,value = %f",*pEncoder_K);
          }
          else
          {
            g_EnvCfg.sENV.f.sF.encoder_k.data = last_value;
            sdlog_printf_app("flash set defult reset_odom Fail,value = %f",*pEncoder_K);
          }
        } 
                    
        return;
    }

    //如果flash中不存在一个有效地数据，则使用默认值
    if(buf > 10.0f || buf < 0.1f )
    {
        GR_LOG_WARN("0.1<buf>10 encoder scale is defult is: %f",*pEncoder_K);
        return;
    }

    *pEncoder_K = buf;
    limitRange2(*pEncoder_K,0.1f,10.0f);
    GR_LOG_INFO("encoder odom is: %f",*pEncoder_K);
}

/*************************************************
Function:     
reset_odom_encoder
Description:    标定odom 编码值
Input:  encoder :标定数值
*************************************************/
float reset_odom_encoder(float encoder)
{
    if(encoder==0)
    {
        defaultMotorPara();
        sdlog_printf_app("set odom default motor=%d ,odom = %f"
            ,motor_parameter.motor_type,motor_parameter.Encoder_K);
        GR_LOG_INFO("set odom default motor=%d ,odom = %f"
            ,motor_parameter.motor_type,motor_parameter.Encoder_K);
    }
    else
    {
        //查询数据开始位置
        sdlog_printf_app("encoder_k = %f",encoder);
        motor_parameter.Encoder_K*=encoder;
        limitRange2(motor_parameter.Encoder_K,0.8f,10.0f);//值合法约束
        sdlog_printf_app("real encoder scale is: %f",motor_parameter.Encoder_K);
    }
    return motor_parameter.Encoder_K;
}

/*************************************************
Function:       readDriverFlash
Description:    从flash读取 driver 参数
return :        driver 
*************************************************/
uint8_t readDriverFlash(void)
{
    uint8_t driver;
    if((!isnormal(g_EnvCfg.sENV.type.sTYPE.driver_type))
            ||(g_EnvCfg.sENV.type.sTYPE.driver_type > KEYA) )
    {
        driver = KEYA;
    }
    else
    {
        driver = g_EnvCfg.sENV.type.sTYPE.driver_type;
    }
    return driver;
}

DRIVER_PARAMETER_ *Read_MotorDriverType(void)
{
    char *str;
    uint8_t motorDriverType;
    str = read_env_app("driver");
    if(str == NULL)
    {
        motorDriverType = readDriverFlash();
        GR_LOG_INFO("flash driver read = %d",motorDriverType);
    }
    else
    {
        //能确保SD里面的参数是正确的吗？
        motorDriverType = atoi(str);
        if(motorDriverType > MOTER_DEIVER_MAX)
        {
            motorDriverType = readDriverFlash();
            GR_LOG_INFO("have sd,but read flash driver = %d",motorDriverType);
        }
        else
        {
            GR_LOG_INFO("SD driver read = %d",motorDriverType);
        }
    }

    switch(motorDriverType)
    {
    case BLD_IO:
        driver_parameter.motor_driver_type = BLD_IO;
        driver_parameter.motor_driver_control_type = MOTOR_DRIVER_VOLTAGE_CTL;
        break;
    case KEYA:
        driver_parameter.motor_driver_type = KEYA;
        driver_parameter.motor_driver_control_type = MOTOR_DRIVER_VOLTAGE_CTL;
        break;
    default:
        driver_parameter.motor_driver_type = BLD_IO;
        driver_parameter.motor_driver_control_type = MOTOR_DRIVER_VOLTAGE_CTL;
        break;
    }

    return &driver_parameter;

}
/*************************************************
Function:       readChassisFlash
Description:    从flash读取 chassis 参数
return :        chassis 
*************************************************/
uint8_t readChassisFlash(void)
{
    uint8_t chassis = 0;
    if((!isnormal(g_EnvCfg.sENV.type.sTYPE.chassis_type))
            ||(g_EnvCfg.sENV.type.sTYPE.chassis_type > ROAM) )
    {
        chassis = ROAM;
    }
    else
    {
        chassis = g_EnvCfg.sENV.type.sTYPE.chassis_type;
    }
    return chassis;
}

CHASSIS_PARAMETER_ *Read_ChassisType(void)
{
    char *str;
    str = read_env_app("chassis");

    //发现如果是读flash,后续参数不会更新，故拆分这个if wqw 19/8/14
    if(str == NULL)
    {
        chassis_parameter.chassis_type = readChassisFlash();
        GR_LOG_INFO("flash chassis read = %d", chassis_parameter.chassis_type);
    }
    else
    {
        //能确保SD里面的参数是正确的吗？
        chassis_parameter.chassis_type = atoi(str);
        if (chassis_parameter.chassis_type > CHASSIS_MAX)
        {
            chassis_parameter.chassis_type = readChassisFlash();
            GR_LOG_INFO("have sd,but read flash chassis = %d", chassis_parameter.chassis_type);
        }
        else
        {
            GR_LOG_INFO("SD chassis read = %d",chassis_parameter.chassis_type);
        }
    }

    if(chassis_parameter.chassis_type == WHEEL_CHAIR1)
    {
        chassis_parameter.imu_parameter_Ptr.GYRO_X_ADJUST_ = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_ADJUST_ = 0;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_ADJUST_ = 2;
        chassis_parameter.imu_parameter_Ptr.GYRO_X_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_NEG_    = -1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_X_ADJUST_  = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_ADJUST_  = 0;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_ADJUST_  = 2;
        chassis_parameter.imu_parameter_Ptr.ACC_X_NEG_     = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_NEG_     = -1;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_NEG_     = 1;
        configImuPara(&chassis_parameter.imu_parameter_Ptr);
        chassis_parameter.wheel_separation = 0.55f;
    }
    else if(chassis_parameter.chassis_type == WHEEL_CHAIR2)
    {
        chassis_parameter.imu_parameter_Ptr.GYRO_X_ADJUST_ = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_ADJUST_ = 0;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_ADJUST_ = 2;
        chassis_parameter.imu_parameter_Ptr.GYRO_X_NEG_    = -1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_X_ADJUST_  = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_ADJUST_  = 0;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_ADJUST_  = 2;
        chassis_parameter.imu_parameter_Ptr.ACC_X_NEG_     = -1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_NEG_     = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_NEG_     = 1;
        configImuPara(&chassis_parameter.imu_parameter_Ptr);
        chassis_parameter.wheel_separation = 0.562f;
    }
    else if(chassis_parameter.chassis_type == ROAM)
    {
        chassis_parameter.imu_parameter_Ptr.GYRO_X_ADJUST_ = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_ADJUST_ = 0;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_ADJUST_ = 2;
        chassis_parameter.imu_parameter_Ptr.GYRO_X_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Y_NEG_    = -1;
        chassis_parameter.imu_parameter_Ptr.GYRO_Z_NEG_    = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_X_ADJUST_  = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_ADJUST_  = 0;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_ADJUST_  = 2;
        chassis_parameter.imu_parameter_Ptr.ACC_X_NEG_     = 1;
        chassis_parameter.imu_parameter_Ptr.ACC_Y_NEG_     = -1;
        chassis_parameter.imu_parameter_Ptr.ACC_Z_NEG_     = 1;
        configImuPara(&chassis_parameter.imu_parameter_Ptr);
        chassis_parameter.wheel_separation = 1.065f;
    }

    return &chassis_parameter;
}
