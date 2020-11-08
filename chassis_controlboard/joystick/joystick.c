#include "joystick.h"
#include "idea.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Motor_Control.h"
#include "Subscribe_Pc.h"
#include "chassis_fsm.h"
#include "Subscribe_sensorBoard.h"
#include "SD_Log_Driver.h"
#include <stdio.h>

#define K1_Acc 0.1f
#define K2_Acc (1-K1)

#define K1 0.1f
#define K2 (1-K1)

#define JOY_A_BUF_LENGTH 60				//角速度Buff长度
#define JOY_L_BUF_LENGTH 30       //线速度Buff长度

float filterBuf[JOY_L_BUF_LENGTH]= {0};
float filterBuf_Acc[JOY_A_BUF_LENGTH]= {0};
float error_inside[JOY_L_BUF_LENGTH]= {0};
static eJOYS_STATE m_joysState;//告警状态

static Joystick_TypeDef sJoyCalData;
float ready_speed_line = 0;
float ready_speed_an = 0;

MOTORCONTROL_Typedef motor_inst_Joy;
static float buf_x, buf_y;
float present_v = 0;

//单纯手动模式，不考虑状态机 ON=不考虑 OFF=考虑状态机
static uint8_t m_joyMod = OFF;

float insDeadBand(float data,float arg);//插入死区
float mapLinear(float arg);						//把线速度的数据映射到归一化的摇杆行程
float mapAngular(float arg);					//把角速度的数据映射到归一化的摇杆行程
/*************************************************
Description:		初始化摇杆状态，在outboard_sensor中调用
Input:
*************************************************/
void initJoystick(void)
{
    float joy_l_max=0,joy_a_max =0,last_value = 0;
	
	 //从SD 卡读取线速度值
    if (false == get_float_SDkey("joy_l",&joy_l_max))
    {
        //判断x是否为一个数（非inf或nan），是返回1，其它返回0；
        if(isnormal(g_EnvCfg.sENV.f.sF.joyReal_l_max.data))
        { 
						joy_l_max = g_EnvCfg.sENV.f.sF.joyReal_l_max.data;
            GR_LOG_WARN("read flash joy_l_max = %f OK",joy_l_max);           
        }
        else
        {
						joy_l_max = JOY_MAX_V_DEFAULT;
            GR_LOG_WARN("read flash joy_l_max err,get default: %f",joy_l_max);

            if(g_EnvCfg.sENV.f.sF.joyReal_l_max.data != joy_l_max)
            {
              last_value = g_EnvCfg.sENV.f.sF.joyReal_l_max.data;
              g_EnvCfg.sENV.f.sF.joyReal_l_max.data = joy_l_max;
              if(set_env_flash(g_EnvCfg) == OK)
              {
                sdlog_printf_app("flash set defult joy_l_max ok,value = %f",joy_l_max);
              }
              else
              {
                g_EnvCfg.sENV.f.sF.joyReal_l_max.data = last_value;
                sdlog_printf_app("flash set defult joy_l_max Fail,value = %f",last_value);
              }
            }
        }
    }
		else
		{
				GR_LOG_WARN("read SD joy_l_max = %f OK",joy_l_max);    
		}
		sJoyCalData.real_y_max = joy_l_max;
		limitRange2(sJoyCalData.real_y_max,JOY_MIN_V,JOY_MAX_V);		

    //从SD 卡读取角速度值
    if (false == get_float_SDkey("joy_a",&joy_a_max))
    {
        if(isnormal(g_EnvCfg.sENV.f.sF.joyReal_a_max.data)) 
        {
						joy_a_max = g_EnvCfg.sENV.f.sF.joyReal_a_max.data;
            GR_LOG_WARN("read flash joy_a_max = %f OK",joy_a_max);            
        }
        else
        {
						joy_a_max = JOY_MAX_A_DEFAULT;
            GR_LOG_WARN("read flash joy_a_max err,get default: %f",joy_a_max);

            if(g_EnvCfg.sENV.f.sF.joyReal_a_max.data != joy_a_max)
            {
              last_value = g_EnvCfg.sENV.f.sF.joyReal_a_max.data;
              g_EnvCfg.sENV.f.sF.joyReal_a_max.data = joy_a_max;
              if(set_env_flash(g_EnvCfg) == OK)
              {
                sdlog_printf_app("flash set defult joy_a_max ok,value = %f",joy_a_max);
              }
              else
              {
                g_EnvCfg.sENV.f.sF.joyReal_a_max.data = last_value;
                sdlog_printf_app("flash set defult joy_a_max Fail,value = %f",last_value);
              }
            }
        }
    }
		else
		{
				GR_LOG_WARN("read SD real_x_max = %f OK",joy_a_max);    
		}
		sJoyCalData.real_x_max = joy_a_max;
		limitRange2(sJoyCalData.real_x_max,JOY_MIN_A,JOY_MAX_A);		
}

/*************************************************
Function: 			setJoyMode
Description:		设置摇杆模式开关 [off=0|on=1]
set_joymod  [0/1]
Input:	cmd : [set_joymod 0|1]   eg. set_joymod 0|1
*************************************************/
void setJoyMode(uint8_t mod)
{
    if (mod != OFF && mod != ON)
    {
        return;
    }
    m_joyMod = mod;
}
/*************************************************
Function: 		getJoyMode
Description:	获取摇杆模式开关 [off=0|on=1]
set_joymod  [0/1] 方便板车的时候用
Input:	cmd : [set_joymod 0|1]   eg. set_joymod 0|1
*************************************************/
uint8_t getJoyMode(void)
{
    return m_joyMod ;
}
/*************************************************
Function: 		getJoystickData
Description:	对外调用 供外部使用摇杆数据
output:	pmsg : 摇杆数据指针
*************************************************/
uint8_t getJoystickData(Joystick_TypeDef *pmsg)
{
    memcpy(pmsg, &sJoyCalData, sizeof(Joystick_TypeDef));
    return 0;
}

/*************************************************
Function: 		filterAlgorithm
Description:	滤波算法
intput:	arg : 输入速度值 status:是否需要滤波 
ON=需要滤波 OFF=不需要滤波
*************************************************/
float filterAlgorithm(float arg, uint8_t status)
{
    float sum = 0;
    float ERROR_ER;
    float peak_error = 0;
    //float ac_para;
    float	peak_para;
    
    //输入
    static int point = 0;
//		static int delayCount = 0;
		
/*刹车功能延时部分****************************/		
		static int joy_breakState = 0;
		static int joy_breakHold = 0;

//3.向前掰摇杆立刻解除刹车状
		static char frontJoyState[2];
	
		frontJoyState[1] = arg>0.15f?1:0;
		if(frontJoyState[1]==1&&frontJoyState[0]==0)
		{
			joy_breakState = 0;
			joy_breakHold = 0;
			for(int i = 0; i<JOY_L_BUF_LENGTH; i++)
			{
				filterBuf[i] = 0;
			}
		}
		frontJoyState[0] = frontJoyState[1];
		
	
		if(joy_breakHold > 0)
		{
			joy_breakHold --;			
			return 0;
		}
/*end*刹车功能延时部分*************************/

    present_v = (controlDataInst.present_v[0] + controlDataInst.present_v[1]) / 2;
    if(status == OFF)
    {
        return arg;
    }

//Solve the bug when active safety occurs, the vehicle speeds up rapidly
    
    float back;
    float front;	
    //速度限定 确保在最小 最大之间
    back  = mapLinear(speed_limit[0][0]);
    front = mapLinear(speed_limit[0][1]);
    limitRange2(arg,back,front);		

    if((point > JOY_L_BUF_LENGTH) || (point == JOY_L_BUF_LENGTH))
    {
        point = 0;
    }
    filterBuf[point] = arg;
    point++;
		
		//解决向后掰摇杆在向前，回中后车向前运动的问题
		if(arg == 0 && present_v == 0)
		{
				for(int i = 0; i<JOY_L_BUF_LENGTH; i++)
				{
					filterBuf[i] = 0;
				}
		}
		
/*为了解决在地理围栏限速后，突然放开限速会有平滑buff中的数据干扰
**************************************************************/
		
		
		static unsigned int lastSLimitFront[2];
		static unsigned int lastSLimitBack[2];

    //保存当前速度限定是否不为0的标志
		lastSLimitFront[0] = speed_limit[0][1]!=0?1:0;
		lastSLimitBack[0]  = speed_limit[0][0]!=0?1:0;

    //如果上一次速度限定不为，且当前为0 则情况缓冲区
		if( (lastSLimitFront[1]==1 && lastSLimitFront[0]==0) 
			||(lastSLimitBack[1]==1 && lastSLimitBack[0]==0) )
		{
      //目的 只在下发speedlimit为0时清空一次
			for(int i = 0; i<JOY_L_BUF_LENGTH; i++)
			{
					filterBuf[i] = 0;
			}
		}
    //保存到下一次
		lastSLimitFront[1] = lastSLimitFront[0];
		lastSLimitBack[1] = lastSLimitBack[0];		
/*为了解决在地理围栏限速后，突然放开限速会有平滑buff中的数据干扰
end*************************************************************/	

    //求均值
    for(int i = 0; i<JOY_L_BUF_LENGTH; i++)
    {
        sum = sum + filterBuf[i];
    }

    // cruise mode judgement
    float avg_inside = sum / JOY_L_BUF_LENGTH;
    for (int j = 0; j < JOY_L_BUF_LENGTH; j++)
    {
        error_inside[j] = fabs(filterBuf[j] - avg_inside);
        if (error_inside[j] > peak_error)
        {
            peak_error = error_inside[j];
        }
    }
    if (peak_error > 0.4f)
    {
        peak_error = 0.4f;
    }
    // ac_para 当前车速加速度调整因子
    //ac_para = 0.3f * (present_v - 1.5f) * (present_v - 1.5f) + 0.3f;
    // peak_para buffer离差极值加速度抑制因子
    peak_para = 10.0f * (peak_error - 0.3f) * (peak_error - 0.3f) + 0.1f;
    if (present_v < 0.4f)
    {
        //ac_para = 1.0f;
        peak_para =1.0f;
    }
    sum -= arg;

    sum = sum/(JOY_L_BUF_LENGTH-1);
    sum *= K2;
    sum = sum + K1*arg;
    ERROR_ER = fabs(arg - sum);
    if (ERROR_ER > 1.0f)
    {
        ERROR_ER = 1.0f;
    }
    if((sum < (MAX_LINEAR_FRONT_SPEED * 0.3f)&&(sum>0)))
    {
        sum = sum - (ERROR_ER * ERROR_ER * 0.1f);
    }

    // cruise mode settlement
    // sum = controlDataInst.goalLinear_v_pre + (sum - controlDataInst.goalLinear_v_pre) * ac_para * peak_para;
    if (sum > 0)
    {
        sum = controlDataInst.goalLinear_v_pre + (sum * sJoyCalData.real_y_max - controlDataInst.goalLinear_v_pre) * peak_para;
        sum = sum / sJoyCalData.real_y_max;
    }
		
		
		/*增加使减速更快的代码*/
		static int debug_i = 10;
		
		if((sum>0 && sum > arg)||(sum < 0 && sum < arg))
		{
			int point_buf;
			point_buf = point - 2;
			for(int i=0;i<debug_i;i++)
			{			
				if(point_buf<0)
				{
					point_buf += JOY_L_BUF_LENGTH;
				}
				filterBuf[point_buf] = arg;
				point_buf--;
			}
		}				
		
		/*end*增加使减速更快的代码*/	
		
/*增加刹车代码*/
		//1.检查刹车特征
		if(present_v>0.3f && arg < 0.1f)
		{
			joy_breakState ++;
		}
		else
		{
			joy_breakState = 0;
		}
		
		//2.在sum为0后进入刹车状态，维持4s				
		if(joy_breakState==5&&sum<=0)
		{
			sum = 0;
			joy_breakHold = 20;
		}		
/*end增加刹车代码*/
		
    return sum;

}
//方式 调整平滑强度
//降低最大值
/*************************************************
Function: 		filterAcc
Description:	方式 调整平滑强度  降低最大值
intput:	x : 输 status:是否需要滤波 
ON=需要滤波 OFF=不需要滤波
*************************************************/
float filterAcc(float arg, uint8_t status)
{
    float sum_acc = 0;
    float ERROR_ER_acc;

    //输入
    static int point_acc = 0;

    if(status==OFF) 
    {
        //不需要滤波 直接退出
        return arg;
    }
    
    //Solve the bug when active safety occurs
    //the vehicle speeds up rapidly
    
		float left;
		float right;
		
		right = mapAngular(speed_limit[1][0]);
		left = mapAngular(speed_limit[1][1]);
		
		limitRange2(arg,right,left);
			
    if((point_acc>JOY_A_BUF_LENGTH)||(point_acc ==JOY_A_BUF_LENGTH))
    {
        point_acc = 0;
    }
    filterBuf_Acc[point_acc] = arg;
    point_acc++;

/*解决摇杆左右打，回中停止后，车又在移动的问题*/		
		if(arg == 0 && present_v == 0)
		{
				for(int i = 0; i<JOY_A_BUF_LENGTH; i++)
				{
					filterBuf_Acc[i] = 0;
				}
		}
/*end*解决摇杆左右打，回中停止后，车又在移动的问题*/
		

    //求均值
    for(int i = 0; i<JOY_A_BUF_LENGTH; i++)
    {
        sum_acc = sum_acc + filterBuf_Acc[i];
    }
    sum_acc -= arg;
		sum_acc = sum_acc/(JOY_A_BUF_LENGTH-1);
						   
    sum_acc = K2_Acc*sum_acc + K1_Acc*arg;
		
    ERROR_ER_acc = fabs(arg - sum_acc);
    if (ERROR_ER_acc > 0.6f)
    {
        ERROR_ER_acc = 0.6f;
    }
    if(sum_acc < MAX_ANGULAR_SPEED * 0.5f)
    {
        sum_acc = sum_acc - (ERROR_ER_acc * ERROR_ER_acc * 0.06f);
    }
		
		if((controlDataInst.present_v[0]+controlDataInst.present_v[1])/2.0f>0.5f)
		{
			if((point_acc>JOY_A_BUF_LENGTH)||(point_acc ==JOY_A_BUF_LENGTH))
			{
					point_acc = 0;
			}
			filterBuf_Acc[point_acc] = arg;
			point_acc++;
		}
    
    return sum_acc;
}
/*************************************************
Function: 		mapToControlCurve
Description:	映射控制值
intput:	x : 输入线速度值
*************************************************/
float mapToControlCurve(float x)
{
    float y;
    if(x>0) //映射到最大速度
    {
        y = x * JOY_MAX_V_DEFAULT;
    }
    else		//后退限速0.3m/s
    {
        y = x*JOY_A1_VALUE/JOY_A1;
    }
    return y;
}

float stopOffset(float x)		//用于降低刹车顿感
{
    static float y;
    static unsigned state=0;
//	static float joyDir = 0;//0 无方向，1 正，-1 负

//	if(x>0.1f)
//	{
//		joyDir = 1;
//	}
//	if(x<-0.1)
//	{
//		joyDir = -1;
//	}

//	float debug_buf = 0.0;
    switch(state)
    {
    case 0:
        y = x;
        if(fabs(x)>STOP_OFFSET_POINT)
            state = 1;
        break;
    case 1:
        if(fabs(x)<STOP_OFFSET_POINT)
        {
            state = 2;
            break;
        }
        y = x;
        break;
    case 2:
        if(fabs(y) < fabs(x))
            y = x;
        else
            y += JOY_FILT_Ts2*(x-y);	//补偿时间常数0.03
        if(fabs(x)>STOP_OFFSET_POINT)
            state = 1;
        if(fabs(y)<0.03)
            state = 0;
    default:
        break;
    }
    return y;
}

// 发送摇杆事件
void sendJoystickEvtToChassisFsm(void)
{
    if((getChassisFsmCurState() == CHASSIS_STATE_READY) || (getChassisFsmCurState() == CHASSIS_STATE_AUTO))
    {
        sendEvtToChassisFsm(FSM_EVT_JOYSTICK);

        //达到自动驾驶时，自动关闭
        setJoyMode(OFF);

        GR_LOG_INFO("fsm joystick event get, cur state = %d.", getChassisFsmCurState());
    }
}
/*************************************************
Function: 		handleJoystickEvent
Description:	产生摇杆事件状态通知
intput:	none
*************************************************/
void handleJoystickEvent(void)
{
    static uint8_t joystick_num = 0;

    if((sJoyCalData.offset_val_x > 0.2f) || (sJoyCalData.offset_val_x < -0.2f)
     || (sJoyCalData.offset_val_y >0.2f) || (sJoyCalData.offset_val_y < -0.1f))
    {
        joystick_num++;
        if(joystick_num >= 10)  //Discriminate multiple times
        {
            joystick_num = 10;
            sendJoystickEvtToChassisFsm();
        }
    }
    else
    {
        joystick_num = 0;
    }
}

/*************************************************
Function: 			filterFsm_an
Description:		角速度速度滤波
ouput:	*bufX
*************************************************/
void filterFsm_an(float *bufX)
{
    //*bufX = g_chassisFsm.pState->filter();
    
    if (getChassisFsmCurState() == CHASSIS_STATE_MANNUAL
            || m_joyMod == ON)
    {
        *bufX = filterAcc(sJoyCalData.pre_x,ON);
        return;
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_AUTO)
    {
        *bufX = filterAcc(sJoyCalData.pre_x,OFF);
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_READY)
    {
        float Exp_speed_An = filterAcc(0,ON);
        if(Exp_speed_An > 0.1f)
        {
            ready_speed_an = filterAcc(0,ON);
        }
        else
        {
            *bufX = filterAcc(sJoyCalData.pre_x,OFF);
        }
    }
    else
    {
        *bufX = 0;
    }
}
/*************************************************
Function: 			filterFsm_line
Description:		线速度速度滤波
ouput:	*bufY
*************************************************/
void filterFsm_line(float *bufY)
{
    if (getChassisFsmCurState() == CHASSIS_STATE_MANNUAL
            || (m_joyMod == ON))
    {
        *bufY = filterAlgorithm(sJoyCalData.pre_y,ON);
        return;
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_AUTO)
    {
        *bufY = filterAlgorithm(sJoyCalData.pre_y,OFF);
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_READY)
    {
        float Exp_speed_Ln = filterAlgorithm(0,ON);
        if(Exp_speed_Ln > 0.1f)
        {
            ready_speed_line = filterAlgorithm(0,ON);
        }
        else
        {
            *bufY = filterAlgorithm(sJoyCalData.pre_y,OFF);
        }
    }
    else
    {
        *bufY = 0;
    }
}

/*************************************************
Function: 			readJoystickDevice
Description:		读取摇杆设备数据
output:	*void
*************************************************/
uint8_t readJoystickDevice(void)
{
    static uint8_t time_read = 0;
    float buf_y2;

    time_read++;
    //获取摇杆数据（只读取x轴和有轴） 及状态
    m_joysState.u8_v = getJoystickRawData(&sJoyCalData);
    //更新电机相关数据参数
    updateMotorData(&motor_inst_Joy);

    //设置死区 小于JOY_DEADBAND   :0.12f值时，摇杆数据为0 缩放百分之几，映射
		sJoyCalData.pre_x = insDeadBand((sJoyCalData.val_x - sJoyCalData.Init_val_x)/95.0f,JOY_DEADBAND);
		sJoyCalData.pre_y = insDeadBand((sJoyCalData.val_y - sJoyCalData.Init_val_y)/95.0f,JOY_DEADBAND);

    //根据电机类型 做滤波
    if( (motor_inst_Joy.motor_parameter_Ptr->motor_type == MOTOR_JIQIN90N )
       || (motor_inst_Joy.motor_parameter_Ptr->motor_type == MOTOR_LIANYI60N)
			 || (motor_inst_Joy.motor_parameter_Ptr->motor_type == MOTOR_LIANYI_NEW)
       || (motor_inst_Joy.motor_parameter_Ptr->motor_type == MOTOR_ZHAOYANG))
    {
         //角速度速度滤波
        filterFsm_an(&buf_x);
        if(time_read == 5)
        {
          //线速度滤波
            filterFsm_line(&buf_y);
            time_read = 0;
        }
    }
    else
    {
        // /=95;缩放 因为发过来的数据可能最大不到100
        buf_x += JOY_FILT_Ts1*(sJoyCalData.pre_x - buf_x);
        if(time_read == 5)
        {
            buf_y += JOY_FILT_Ts1*(sJoyCalData.pre_y - buf_y);
            time_read = 0;
        }
    }
		
    limitRange(buf_y,1);
    limitRange(buf_x,1);
    sJoyCalData.offset_val_x = buf_x*sJoyCalData.real_x_max;
		
    buf_y2 = mapToControlCurve(buf_y); //运动曲线分段
    
		if(buf_y2>0)
			sJoyCalData.offset_val_y = buf_y2*(sJoyCalData.real_y_max/JOY_MAX_V_DEFAULT);//stopOffset(buf_y2);
		else
			sJoyCalData.offset_val_y = buf_y2;
//	debugJoy1 = debugJoy[1] * 1000;
		
/*构造一组新的代码，改变以获得摇杆的原始值 （映射好的）
		******************************************************/
		float joy_x = 0;
		float joy_y = 0;
		joy_y = sJoyCalData.pre_y;       
		joy_x = sJoyCalData.pre_x;           
		limitRange(joy_y,1);
    limitRange(joy_x,1);
		sJoyCalData.real_x = joy_x * sJoyCalData.real_x_max;
		sJoyCalData.real_y = mapToControlCurve(joy_y)*(sJoyCalData.real_y_max/JOY_MAX_V_DEFAULT);
/*end******************************************************/
		
//	debugJoy2 = pmsg->offset_val_y * 1000;

    //避免急停后摇杆控制量累加，急停恢复后弹射起步
    if(motor_inst_Joy.scramFlag)
    {
        buf_y = 0;
        buf_x = 0;
    }

    handleJoystickEvent();
    return m_joysState.u8_v;
}

/*************************************************
Function: 			getJoystickState
Description:		获取摇杆状态 老的告警接口
Input:					none
Output: 				none
Return: 				flag_ ： 数据异常1还是通讯异常2 或者正常0
*************************************************/
uint8_t getJoystickState(void)
{
    return  m_joysState.u8_v;
}
/*************************************************
Function: 			clearJoystickState
Description:		清楚摇杆状态 暂时没用
*************************************************/
void clearJoystickState(void)
{
    m_joysState.u8_v = JOYS_STATE_OK;
}

/*************************************************
Function: 			insDeadBand
Description:		往信号data中插入死区arg         -1<data<1 0<arg<1
*************************************************/
float insDeadBand(float data,float arg)
{
	if(fabs(data)<arg)
	{
		return 0;
	}
	else if(data>arg)
	{
		return (data-arg)/(1.0f-arg);
	}
  else
	{
		return (data+arg)/(1.0f-arg);
	}
}


/*
	把线速度的数据映射到归一化的摇杆行程
*/
float mapLinear(float x)
{
	float y;
	if(x>0) //映射到最大速度
	{
			y = x / sJoyCalData.real_y_max;
	}
	else		//后退限速0.3m/s
	{
			y = x / JOY_A1_VALUE;
	}
	return y;
}
/*
	把角速度的数据映射到归一化的摇杆行程
*/
float mapAngular(float arg)
{
	return arg / sJoyCalData.real_x_max;
}

