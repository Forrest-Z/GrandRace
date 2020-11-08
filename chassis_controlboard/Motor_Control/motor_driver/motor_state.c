#include "motor_state.h"
#include "SD_Log_App.h"
#include "Bms.h"

unsigned char clearMotorStateEN = 0;

//私有函数
void resetMotorState(MOTORCONTROL_Typedef *pmsg);
unsigned int detectBrakeErr(MOTORCONTROL_Typedef *motor_inst);
unsigned int detectMotorFail(MOTORCONTROL_Typedef *motor_inst,unsigned int motorAddr);

unsigned int readMotorState(MOTORCONTROL_Typedef *motor_inst)
{
    if(motor_inst->BRK||motor_inst->lock||motor_inst->scramFlag)
    {   //防止在刹车时，急停按下时发生电机失效判断
        motor_inst->set_speed[0] = 0;
        motor_inst->set_speed[1] = 0;
    }
		
	
	detectBrakeErr(motor_inst);
	detectMotorFail(motor_inst,0);
	detectMotorFail(motor_inst,1);
		
		
	static uint8_t scramLast=0;
	if( scramLast != motor_inst->scramFlag )
	{
		if(motor_inst->scramFlag)
		{
			clearMotorStateEN = 0;
			motor_inst->state=0;
			motor_inst->HALLFlag[0] = 0;
			motor_inst->HALLFlag[1] = 0;
			GR_LOG_ERROR("clear all of motor err");
			clearAlarm(eA_MOTOR);
		
			motor_inst->state |= MOTOR_STATE_ERR_SCRAM;
			setAlarm(eA_MOTOR,eb04_emgyStopFatal,ALARM_ERR);
			GR_LOG_ERROR("scram is pressed");										
		}
		else
		{
			motor_inst->state &= ~MOTOR_STATE_ERR_SCRAM;//急停打开
			setAlarm(eA_MOTOR,eb04_emgyStopFatal,ALARM_OK);
			GR_LOG_WARN("scram is open rocover");
		}	
	}
	scramLast = motor_inst->scramFlag;
		
	return motor_inst->state;
}

//    static unsigned char status_left = 0;
//    static unsigned char left_flag[2];//失效标志，反转标志
//    static unsigned char status_Right = 0;
//    static unsigned char right_flag[2];//失效标志，反转标志
//    static float distance_buf[2]= {0,0}; //用于缓存里程做逆转检测
//    static float left_lock = 0;
//    static float right_lock = 0;
//    /**********************************************************************/
//		static bool b_Lbrake = false,b_Rbrake = false; //左右电机驻车刹车失效日志标志
//		
//    if(clearMotorStateEN)//复位电机检测
//    {
//        clearMotorStateEN = 0;
//        motor_inst->state=0;
//        status_left = 0;
//        status_Right = 0;
//        left_flag[0] = 0;
//        right_flag[0] = 0;
//        left_flag[1] = 0;
//        right_flag[1] = 0;
//        left_lock = motor_inst->distance[0];
//        right_lock = motor_inst->distance[1];
//        motor_inst->HALLFlag[0] = 0;
//        motor_inst->HALLFlag[1] = 0;
//				clearAlarm(eA_MOTOR);
//    }

//    if(motor_inst->BRK||motor_inst->lock||motor_inst->scramFlag)
//    {   //防止在刹车时，急停按下时发生电机失效判断
//        motor_inst->set_speed[0] = 0;
//        motor_inst->set_speed[1] = 0;
//    }

//    /**********************************************************************/

//    //左轮检测
//    static unsigned char left_PWM_Count = 0;//滞回，提高检测效率
//    if(fabs(motor_inst->set_speed[0])<0.5)
//    {
//        left_PWM_Count++;
//        if(left_PWM_Count>10)
//        {
//            left_PWM_Count = 100;
//            status_left = 0;
//        }
//    } else
//    {
//        left_PWM_Count = 0;
//    }
//    switch(status_left)
//    {
//        static unsigned count = 0;
//    case 0:
//        if(fabs(motor_inst->set_speed[0])>0.5)//提高检测的门限
//        {
//            status_left = 1;
//            count = 0;
//            left_flag[0] = 0;//避免在频繁，短时触发累计到这个量
//        }
//        break;
//    case 1:
//        count++;
//        if(count>50)
//        {
//            count = 0;
//            distance_buf[0] = motor_inst->distance[0];
//            status_left = 2;
//        }
//        break;
//    case 2:
//        count++;
//        if(count>9)//开始检测状态，采样率10Hz
//        {
//            count = 0;
//            /*失效检测*/
//            if(fabs(motor_inst->real_speed[0])<=0.05)
//            {
//                left_flag[0]++;
//                if(left_flag[0]>10)
//                {
//                    left_flag[0] = 10;//采样到电机无反应
//                }
//            }
//            else
//            {
//                left_flag[0] = 0;
//            }
//            /*失效检测结束*/
//        }
//        /*逆转检测*/
//        static unsigned char left_distance_flag = 0;//控制交错的时候需要更新路程记录
//        if(motor_inst->set_speed[0]>0)
//        {
//            if(left_distance_flag==0)
//            {
//                distance_buf[0] = motor_inst->distance[0];
//            }
//            left_distance_flag = 1;
//            if((motor_inst->distance[0]-distance_buf[0])<-0.6f)
//            {
//                left_flag[1] = 10;
//            }
//        } 
//        else
//        {
//            if(left_distance_flag==1)
//            {
//                distance_buf[0] = motor_inst->distance[0];
//            }
//            left_distance_flag = 0;
//            if((motor_inst->distance[0]-distance_buf[0])>0.6f)
//            {
//                left_flag[1] = 10;
//            }
//        }
//        /*逆转检测结束*/
//        break;
//    default:
//        break;
//    }
//    //左轮检测 end


//    //右轮检测
//    static unsigned char right_PWM_Count = 0;//滞回，提高检测效率
//    if(fabs(motor_inst->set_speed[1])<0.5f)
//    {
//        right_PWM_Count++;
//        if(right_PWM_Count>10)
//        {
//            right_PWM_Count = 100;
//            status_Right = 0;
//        }
//    }
//    else
//    {
//        right_PWM_Count = 0;
//    }
//    switch(status_Right)
//    {
//        static unsigned count = 0;
//    case 0:
//        if(fabs(motor_inst->set_speed[1])>0.5f)
//        {
//            status_Right = 1;
//            right_flag[0] = 0;//避免在频繁，短时触发累计到这个量
//            count = 0;
//        }
//        break;
//    case 1:
//        count++;
//        if(count>50)//1s
//        {
//            count = 0;
//            distance_buf[1] = motor_inst->distance[1];
//            status_Right = 2;
//        }
//        break;
//    case 2:
//        count++;
//        if(count>9)//开始检测状态，采样率20Hz
//        {
//            count = 0;
//            /*失效检测*/
//            if(fabs(motor_inst->real_speed[1])<=0.05f)
//            {
//                right_flag[0]++;
//                if(right_flag[0]>10.0f)
//                {
//                    right_flag[0] = 10.0f;//采样到电机无反应
//                }
//            }
//            else
//            {
//                right_flag[0] = 0;
//            }
//            /*失效检测结束*/
//        }

//        /*逆转检测*/
//        static unsigned char right_distance_flag = 0;//控制交错的时候需要更新路程记录
//        if(motor_inst->set_speed[1]>0)
//        {
//            if(right_distance_flag==0)
//            {
//                distance_buf[1] = motor_inst->distance[1];
//            }
//            right_distance_flag = 1;
//            if((motor_inst->distance[1]-distance_buf[1])<-0.6f)
//            {
//                right_flag[1] = 10;
//            }
//        } else
//        {
//            if(right_distance_flag==1)
//            {
//                distance_buf[1] = motor_inst->distance[1];
//            }
//            right_distance_flag = 0;
//            if((motor_inst->distance[1]-distance_buf[1])>0.6f)
//            {
//                right_flag[1] = 10;
//            }
//        }
//        /*逆转检测结束*/
//        break;
//    default:
//        break;
//    }
//    //右轮检测 end



//    //组合逻辑
//    if(left_flag[0]>=10)
//    {
//        motor_inst->state |= MOTOR_STATE_ERR_FAIL_L;
//				setAlarm(eA_MOTOR,eb06_lFailMoveFatal,ALARM_ERR);
//    }
//    if(right_flag[0]>=10)
//    {
//        motor_inst->state |= MOTOR_STATE_ERR_FAIL_R;
//				setAlarm(eA_MOTOR,eb09_rFailMoveFatal,ALARM_ERR);
//    }
//    if(motor_inst->scramFlag)
//    {
//        motor_inst->state |= MOTOR_STATE_ERR_SCRAM;
//				setAlarm(eA_MOTOR,eb04_emgyStopFatal,ALARM_ERR);
//    }
//    else
//    {
//				motor_inst->state &= ~MOTOR_STATE_ERR_SCRAM;//急停打开
//				setAlarm(eA_MOTOR,eb04_emgyStopFatal,ALARM_OK);
//    }

//    //需要在这里注册可以检测这些的驱动器（霍尔信号直接接入了控制板）
//		//长期没有跟踪 2019/6/14 wqw
//    if(motor_inst->driver_parameter_Ptr->motor_driver_type == BLD_IO)
//    {
//        if(left_flag[1]>=10)
//        {
//            motor_inst->state |= MOTOR_STATE_ERR_OUT_CTR_L;
//						setAlarm(eA_MOTOR,eb07_lOutCtrlFatal,ALARM_ERR);
//        }
//        if(right_flag[1]>=10)
//        {
//            motor_inst->state |= MOTOR_STATE_ERR_OUT_CTR_R;
//						setAlarm(eA_MOTOR,eb10_rOutCtrlFatal,ALARM_ERR);
//        }
//        if(motor_inst->HALLFlag[0])			//左电机HALL错误
//        {
//            motor_inst->state |= MOTOR_STATE_ERR_HALL_L;
//						setAlarm(eA_MOTOR,eb05_lHallFatal,ALARM_ERR);
//        }
//        if(motor_inst->HALLFlag[1])
//        {
//            motor_inst->state |= MOTOR_STATE_ERR_HALL_R;
//						setAlarm(eA_MOTOR,eb08_rHallFatal,ALARM_ERR);
//        }
//    } 
//    else
//    {
//        motor_inst->state &= ~(MOTOR_STATE_ERR_OUT_CTR_L |
//                               MOTOR_STATE_ERR_OUT_CTR_R |
//                               MOTOR_STATE_ERR_HALL_L |
//                               MOTOR_STATE_ERR_HALL_R);			
//			setAlarm(eA_MOTOR,eb07_lOutCtrlFatal,ALARM_OK);
//			setAlarm(eA_MOTOR,eb10_rOutCtrlFatal,ALARM_OK);
//			setAlarm(eA_MOTOR,eb05_lHallFatal,ALARM_OK);
//			setAlarm(eA_MOTOR,eb08_rHallFatal,ALARM_OK);
//    }

//    //刹车失效检测
////    if(motor_inst->lock)
////    {
////        if(fabs(motor_inst->distance[0]-left_lock)>0.3)
////        {
////            //为了演示 2019/4/8
////					//需要验证代码 2019/6/14
////          motor_inst->state |= MOTOR_STATE_ERR_BRK_FAIL_L;
////					setAlarm(eA_MOTOR,eb01_lParkBrakeErr,ALARM_ERR);
////				
////					if (!b_Lbrake )
////					{
////						GR_LOG_ERROR("motor left park brake invalid");
////						b_Lbrake = true;
////					}
////				
////        }
////        if(fabs(motor_inst->distance[1]-right_lock)>0.3)
////        {
////          motor_inst->state |= MOTOR_STATE_ERR_BRK_FAIL_R;
////					setAlarm(eA_MOTOR,eb03_rParkBrakeErr,ALARM_ERR);
////					if (!b_Rbrake )
////					{
////						GR_LOG_ERROR("motor right park brake invalid");
////						b_Rbrake = true;
////					}
////        }
////    }
////    else
////    {
////        left_lock = motor_inst->distance[0];
////        right_lock = motor_inst->distance[1];
////        motor_inst->state &= ~(MOTOR_STATE_ERR_BRK_FAIL_L | MOTOR_STATE_ERR_BRK_FAIL_R);
////				setAlarm(eA_MOTOR,eb01_lParkBrakeErr,ALARM_OK);
////				setAlarm(eA_MOTOR,eb03_rParkBrakeErr,ALARM_OK);
////				b_Lbrake = false;
////				b_Rbrake = false;
////    }

//    return motor_inst->state;
//}

void resetMotorState(MOTORCONTROL_Typedef *pmsg)
{
    clearMotorStateEN = 1;
    readMotorState(pmsg);
}

void clearMotorState()
{
    clearMotorStateEN = 1;
}

/*以下是一些实时的检测算法
*****************************************************************/
float break_delay = 1.0f; //(s)
float brk_err_th = 0.5f; //0.5m
unsigned int detectBrakeErr(MOTORCONTROL_Typedef *motor_inst)
{
	static float timer_delay;
	static unsigned int state = 0;
	static float distance[2];
  static uint8_t key_flag[2] = {0,0};
	
	if (motor_inst->lock == 0)//全局复位
	{
		state = 0;
	}
	
	switch(state)
	{
		
		case 0:					//驻车刹车没有落下
			if(motor_inst->lock==1)
			{
				state = 1;
				timer_delay = 0;
				key_flag[0] = 0;
				key_flag[1] = 0;
			}
			break;
		case 1:					//驻车延时检测
			timer_delay += motor_inst->Ts;
			if(timer_delay > break_delay)
			{
				state = 2;
				distance[0] = motor_inst->distance[0];
				distance[1] = motor_inst->distance[1];
			}			
			break;
		case 2:       //确认已经完全静止了			
			
			if ( fabs(distance[0] - motor_inst->distance[0]) 
                        > brk_err_th && key_flag[0] == 0)
			{
				setAlarm(eA_MOTOR,eb01_lParkBrakeErr,ALARM_ERR);				
				GR_LOG_ERROR("motor left park brake invalid");
				motor_inst->state |= MOTOR_STATE_ERR_BRK_FAIL_L;				
				key_flag[0] = 1;
			}
			if ( fabs(distance[1] - motor_inst->distance[1]) 
                        > brk_err_th && key_flag[1] == 0)
			{
				setAlarm(eA_MOTOR,eb03_rParkBrakeErr,ALARM_ERR);				
				GR_LOG_ERROR("motor right park brake invalid");
				motor_inst->state |= MOTOR_STATE_ERR_BRK_FAIL_R;
				key_flag[1] = 1;
			}				
		break;
		
	}
	return motor_inst->state;
}

float Motor_ErrTh = 0.2f;//设定与真实执行的误差阈值（m/s）
float ErrDelayTh = 1.5f; //误差超范围允许时间（s）
unsigned int detectMotorFail(MOTORCONTROL_Typedef *motor_inst,unsigned int motorAddr)	//电机运行失败
{
	static unsigned char state[2] = {0,0};
	static float timer_delay[2];
	float controlErr;
  static bool bLog_L = false,bLog_R = false;
	
	controlErr =  fabs(motor_inst->set_speed[motorAddr] 
                  - motor_inst->real_speed[motorAddr]);

   //实际速度大于0.1f 且 控制同向 
	if( controlErr < Motor_ErrTh || (	fabs(motor_inst->real_speed[motorAddr]) 
    > 0.01f  &&  (motor_inst->set_speed[motorAddr] 
    * motor_inst->real_speed[motorAddr])>0.001f)
	)
	{
		state[motorAddr] = 0;
    
    if (motorAddr ==0)
    {
      setAlarm(eA_MOTOR,eb06_lFailMoveFatal,ALARM_OK); 
      if (bLog_L == true)
      {
        bLog_L = false;
			  GR_LOG_WARN("motor left can't move recoer");
      }
      motor_inst->state &= ~MOTOR_STATE_ERR_FAIL_L;
    }
    else if (motorAddr ==1)
    {
      setAlarm(eA_MOTOR,eb09_rFailMoveFatal,ALARM_OK);
      motor_inst->state &= ~MOTOR_STATE_ERR_FAIL_R;
      if(bLog_R == true)
      {
        bLog_R = false;
        GR_LOG_ERROR("motor right can't move recoer");
      }
    }
	}
  
	switch(state[motorAddr])
	{
		case 0:
		{
			if( controlErr > Motor_ErrTh) //发生过大的误差
			{
					state[motorAddr] = 1;
					timer_delay[motorAddr] = 0;
			}
			break;
		}
		case 1:
		{
			timer_delay[motorAddr] += motor_inst->Ts;
      
      //超过一个时间阈值没有回到可以接受的误差内
			if(timer_delay[motorAddr] > ErrDelayTh)	
			{
				state[motorAddr] = 2;
			}
			break;
		}
		case 2:
		{
			if(motorAddr == 0 && bLog_L == false)//左电机
			{
				  setAlarm(eA_MOTOR,eb06_lFailMoveFatal,ALARM_ERR); //报告故障
          motor_inst->state |= MOTOR_STATE_ERR_FAIL_L;
          bLog_L = true;
				  GR_LOG_ERROR("motor left can't move");
			}
			else if (motorAddr == 1 && bLog_R == false)//右电机
			{
          setAlarm(eA_MOTOR,eb09_rFailMoveFatal,ALARM_ERR);
          motor_inst->state |= MOTOR_STATE_ERR_FAIL_R;
          bLog_R = true;
          GR_LOG_ERROR("motor right can't move");
			}
			state[motorAddr] = 3;			
			break;
		}
		case 3:
		{
      if(motorAddr == 0)
      {
        //报告故障
        setAlarm(eA_MOTOR,eb06_lFailMoveFatal,ALARM_ERR); 
        motor_inst->state |= MOTOR_STATE_ERR_FAIL_L;
      }
      else if (motorAddr == 1)
      {
        setAlarm(eA_MOTOR,eb09_rFailMoveFatal,ALARM_ERR);
        motor_inst->state |= MOTOR_STATE_ERR_FAIL_R;
      }
			break;
		}
    default:
    {
      if(motorAddr == 0)
      {
        //报告故障
       // setAlarm(eA_MOTOR,eb06_lFailMoveFatal,ALARM_ERR); 
      }
      else if (motorAddr == 1)
      {
        //setAlarm(eA_MOTOR,eb09_rFailMoveFatal,ALARM_ERR);
      }
      break;
    }
	}
	
	return motor_inst->state;
}	

void readVoltageState(MOTORCONTROL_Typedef *motor_inst)
{
	static float BmsVolt;
	static int count = 0;
	static int count2 = 0;	
	static int stateLast = 0;
	BmsVolt = getBmsTotalVolt();		
	
	if((BmsVolt - motor_inst->input_voltage)>5.0f)
	{
		count ++;
	}
	else
	{
		count = 0;
	}
	
	if(count>2)
	{
		motor_inst->motor_driver |= MOTOR_DRIVER_VOLTAGE_NOT_STABLE;
		count2 = 10*100; //10秒
	}
	else if(count2==0)
	{
		motor_inst->motor_driver &= ~MOTOR_DRIVER_VOLTAGE_NOT_STABLE;
	}
	else
	{
		count2--;
	}
	
	
//检查边沿记录日志	
	if(stateLast^(motor_inst->motor_driver & MOTOR_DRIVER_VOLTAGE_NOT_STABLE))
	{
			if(!stateLast)
			{
				GR_LOG_ERROR("inport voltage not stable");
			}
			else
			{
				GR_LOG_ERROR("inport voltage recover");
			}
	}
	stateLast = (motor_inst->motor_driver & MOTOR_DRIVER_VOLTAGE_NOT_STABLE)?1:0;
	

	
}	
