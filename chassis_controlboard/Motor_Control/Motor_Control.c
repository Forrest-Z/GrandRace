#include "Motor_Control.h"
#include "control_config.h"
#include "active_safety.h"
#include "idea.h"
#include "control_sysErr.h"
#include "fuzzy.h"
#include "smooth.h"
#include "Subscribe_Pc.h"
#include "usart.h"
#include "joystick.h"
#include "Control_selfmodify.h"
/**********************************************************/
//#include "debug_msg.h"
#include "control_debug.h"
#include "Motor_PcConfig.h"
#include "imu.h"
#include "Motor_Control_Monitor.h"
#include "chassis_fsm.h"
#include "SD_Log_App.h"
#include "sys_queue.h"

/***********************************************************/
float goal_export[2]= {0,0};
// 驻车刹车状态
static uint8_t chassis_parking_state = 0;
/***********************************************************/
MOTORCONTROL_Typedef motor_inst;
ControlData_Typedef controlDataInst;

PID_Typedef LeftWheel_PID;
PID_Typedef RightWheel_PID;
PID_Typedef Angular_PID;

void Init_Motor(void)
{
    //初始化 最大速度
    initJoystick();
    motor_inst.motor_parameter_Ptr = Read_MotorType();
    motor_inst.chassis_parameter_Ptr = Read_ChassisType();
    motor_inst.driver_parameter_Ptr = Read_MotorDriverType();

    initMotorDriver(&motor_inst); //初始化驱动层
    initControl();								//初始化控制层
    M_C_MonitorInit(&controlDataInst);
    Init_FuzzyPID();

    GR_LOG_INFO("control initialize is over");
}



int control_main(int arg)
{
    //1.刷新控制参数
    setPIDPara();
    FuzzysetPIDPara(&controlDataInst);

    //2.传感器，控制设定量刷新
    Goal_And_Sensor_Updata(); //-1

    //3.非线性逻辑，驻车刹车逻辑
    Control_Logic();

    //4.传感器及控制设定量预处理
    Control_Preprocess();

    //5 监护代码入口
    M_C_MonitorInterface(&controlDataInst);
		M_C_MonitorLockPower(&controlDataInst);

    //6.角度控制
    Calculate_Wheel_Speed();

    //7.
    Wheel_Control();

    //8.debug
    updateControlDebugdata(&controlDataInst);

    return 0;
}

void initControl()
{
    //左电机PID初值
    LeftWheel_PID.KP = 0.1f;
    LeftWheel_PID.KI = 0.1f;
    LeftWheel_PID.KD = 0.01f;
    LeftWheel_PID.integral_K_Max = 1.0f;
    LeftWheel_PID.EN = 1;

    //右电机PID初值
    RightWheel_PID.KP = 0.1f;
    RightWheel_PID.KI = 0.1f;
    RightWheel_PID.KD = 0.01f;
    RightWheel_PID.integral_K_Max = 1.0f;
    RightWheel_PID.EN = 1;

    //转向PID初值
    switch(motor_inst.motor_parameter_Ptr->motor_type)
    {
    case MOTOR_ORIGINAL:
        Angular_PID.KP = 0.5f;
        Angular_PID.KI = 0.2f;
        Angular_PID.KD = 0.0f;
        break;
    case MOTOR_ZHAOYANG:
        Angular_PID.KP = 0.5f;
        Angular_PID.KI = 0.2f;
        Angular_PID.KD = 0.0f;
        break;
    case MOTOR_JIQIN90N:
        Angular_PID.KP = 0.6f;
        Angular_PID.KI = 0.33f;
        Angular_PID.KD = 0.01f;
        break;
    case MOTOR_LIANYI60N:
        Angular_PID.KP = 0.6f; // 0.6
        Angular_PID.KI = 0.33f; // 0.33
        Angular_PID.KD = 0.01f;
        break;
		case MOTOR_LIANYI_NEW:
        Angular_PID.KP = 0.6f; 		// 0.6
        Angular_PID.KI = 0.33f; 	// 0.33
        Angular_PID.KD = 0.01f;
        break;
    }
    Angular_PID.integral_K_Max = 0.6f;
    Angular_PID.EN = 1;
    controlDataInst.Fuzzy_Control_Enable_flag = 1;

    controlDataInst.Max_front_acc  = MAX_FRONT_ACC;
    controlDataInst.Max_back_acc = MAX_BACK_ACC;
    controlDataInst.Max_angular_acc = MAX_ANGULAR_ACC;

    controlDataInst.Max_front_dec   = MAX_FRONT_DEC;
    controlDataInst.Max_back_dec    = MAX_BACK_DEC;
    controlDataInst.Max_angular_dec = MAX_ANGULAR_DEC;

    controlDataInst.motor_inst_Ptr = &motor_inst;
}

void handleScramEvent(unsigned char scram_flag)
{
    static unsigned char pre_scram_flag = 0xFF;

    if(getTX2CommunicationState() == 0)
    {
        return;
    }

    if(pre_scram_flag != scram_flag)
    {
        if(scram_flag == 1)
        {
            pushChassisEvt(CHASSIS_BREAK_PRESS_EVT);
        }
        else if(scram_flag == 0)
        {
            pushChassisEvt(CHASSIS_BREAK_RECOVERY_EVT);
        }

        pre_scram_flag = scram_flag;
    }
}


void handleParkingEvent(float *speed)
{
    static uint8_t pre_parking_flag = 0xFF;

    if(getTX2CommunicationState() == 0)
    {
        return;
    }

    if(speed == NULL)
    {
        return;
    }

    if((fabs(speed[0]) <= 0.05) && (fabs(speed[1]) <= 0.05) && (pre_parking_flag != 1))
    {
        pushChassisEvt(CHASSIS_PARKING_START_EVT);
        pre_parking_flag = 1;
        chassis_parking_state = 1;
    }
    else if(((fabs(speed[0]) >= 0.1) || (fabs(speed[1]) >= 0.1)) && (pre_parking_flag != 0))
    {
        pushChassisEvt(CHASSIS_PARKING_END_EVT);
        pre_parking_flag = 0;
        chassis_parking_state = 0;
    }
}

uint8_t getParkingState(void)
{
    return chassis_parking_state;
}

void Goal_And_Sensor_Updata()
{
    //	static uint8_t point_realacc = 0;
    float goal_x = 0;
    float goal_y = 0;
    static int state = 0;
    static float lastTimer;			//用作计算执行周期的缓存	
    float remoteControlData[2]= {0,0};

		controlDataInst.Ts = getTs_idea(&lastTimer);	//获取最近一次的采样周期
    Joystick_TypeDef joystickSensor = {0,0,0,0,0,0};

    ImuPara_TypeDef imuSensor;

    //Get sensor data
    updateMotorData(&motor_inst);

    handleScramEvent(motor_inst.scramFlag);
    handleParkingEvent(motor_inst.real_speed);//1

    getJoystickData(&joystickSensor);
    getImuData(&imuSensor);

    //use to debug
    goal_x = goal_export[0];
    goal_y = goal_export[1];

    //Select control source
    if (getChassisFsmCurState() == CHASSIS_STATE_MANNUAL
            || (getJoyMode() == ON) )
    {
        if(state != 0)
        {
            state--;
            goal_y = filterAlgorithm(joystickSensor.offset_val_y,ON);
            goal_x = filterAcc(joystickSensor.offset_val_x,ON);
        }
        else
        {
            goal_x  += joystickSensor.offset_val_x;
            goal_y  += joystickSensor.offset_val_y;
        }

        if(goal_y < speed_limit[0][0])
        {
            goal_y = speed_limit[0][0];
        }
        else if(goal_y > speed_limit[0][1])
        {
            goal_y = speed_limit[0][1];
        }

        if(goal_x < speed_limit[1][0])
        {
            goal_x = speed_limit[1][0];
        }
        else if(goal_x > speed_limit[1][1])
        {
            goal_x = speed_limit[1][1];
        }
        motor_inst.EN = 1;
        ResetRemoteControlData();
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_AUTO)
    {
        state = 10;
        UpdateRemoteControlData(remoteControlData);
        goal_y  += remoteControlData[0];
        goal_x  += remoteControlData[1];

        filterAlgorithm((motor_inst.real_speed[0]+motor_inst.real_speed[1])/2,ON);
        filterAcc(remoteControlData[1],ON);
        motor_inst.EN = 1;
    }
    else if(getChassisFsmCurState() == CHASSIS_STATE_READY)
    {
        goal_x  = 0;
        goal_y  = 0;
        ResetRemoteControlData();
    }
    else
    {
        goal_x  = 0;
        goal_y  = 0;
        filterAlgorithm(0,ON);
        filterAcc(0,ON);
        ResetRemoteControlData();
    }



#if 0
    float inputGoalControlSpeed[2],outputActiveSafetySpeed[2];
    inputGoalControlSpeed[1] = goal_x;
    inputGoalControlSpeed[0] = goal_y;

    active_safetyHanlder(inputGoalControlSpeed,outputActiveSafetySpeed);
    state_machine.activeSafety_state = 2;
    switch(state_machine.activeSafety_state)
    {
    case 0 :
        goal_x = outputActiveSafetySpeed[1];
        goal_y = outputActiveSafetySpeed[0];
        break;
    case 1 :
        if(state_machine.sensor_flag.connectFlag != 0)
        {
            //Get the goal control value
            goal_x = outputActiveSafetySpeed[1];
            goal_y = outputActiveSafetySpeed[0];
        }
        break;
    case 2 :
        //nop
        break;

    default:
        break;
    }
#endif

    //Get the goal control value
    if(motor_inst.motor_parameter_Ptr->motor_type == MOTOR_JIQIN90N)
    {
        //controlDataInst.goalAngular_v = goal_x*1.3f;
        controlDataInst.goalAngular_v = goal_x;
    }
    else if(motor_inst.motor_parameter_Ptr->motor_type == MOTOR_ORIGINAL 
				 || motor_inst.motor_parameter_Ptr->motor_type == MOTOR_ZHAOYANG
				 || motor_inst.motor_parameter_Ptr->motor_type == MOTOR_LIANYI60N
				 || motor_inst.motor_parameter_Ptr->motor_type == MOTOR_LIANYI_NEW )
    {
        controlDataInst.goalAngular_v = goal_x;
    }

    controlDataInst.goalLinear_v  = goal_y;

    //Get the current speed
    controlDataInst.present_v[0] = motor_inst.real_speed[0];
    controlDataInst.present_v[1] = motor_inst.real_speed[1];

    controlDataInst.presentAngular_v = imuSensor.Real_Gyro[2];
    //controlDataInst.presentAngular_v = 0;
    if(motor_inst.chassis_parameter_Ptr->chassis_type == WHEEL_CHAIR2)
    {
        controlDataInst.present_v[0] *= 2;
    }

    /*****目标量过小判0 **********/
    if(fabs(controlDataInst.goalLinear_v)<0.1)
    {
        controlDataInst.goalLinear_v = 0;
        if(fabs(controlDataInst.goalAngular_v)<0.1)
        {
            controlDataInst.goalAngular_v = 0;
        }
    }
    /*end*目标量过小判0 **********/
}

int	Control_Logic()
{

    //若有其它任务停止，则停止电机
    //	static uint8_t monitor_counter = 0;
    //		monitor_counter++;
    //	if(monitor_counter>10)
    //	{
    //		monitor_counter = 0;
    //		control_monitor[0]++;
    //		control_monitor[1]++;
    //	}
    //
    //	if(control_monitor[0]>10)
    //	{
    //		controlDataInst.goalAngular_v = 0;
    //		controlDataInst.goalLinear_v = 0;
    //		printf("Communication task no have looping/n");
    //	}
    //	if(control_monitor[1]>10)
    //	{
    //		controlDataInst.goalAngular_v = 0;
    //		controlDataInst.goalLinear_v = 0;
    //		printf("Sensor task no have looping/n");
    //	}
    //end若有其它任务停止，则停止电机


    if(Brake_Motor())
    {
        controlDataInst.goalLinear_v_pre = 0;
        controlDataInst.goalAngular_v_pre = 0;


        if(controlDataInst.joy_BRK==1)
        {
            motor_inst.BRK = 1;
        }
        return 1;
    }
    if(controlDataInst.joy_BRK==1)
    {
        motor_inst.BRK = 1;
    }
    return 0;
}


void Control_Preprocess()
{
    //	static uint8_t dec_delay_time = 0;
    //	static uint8_t aec_delay_time = 0;
    //	static uint8_t acc_delay_time = 0;
    //	static uint8_t acc_first_flag = 0;
    //	static uint8_t right_delay_time = 0;
    //	static uint8_t left_delay_time = 0;
    //	LineVelocitySmooth(controlData,CTRL_T);
    //	AngularSpeedSmooth(controlData,CTRL_T);

    //	controlDataInst.goalLinear_v_pre += 0.1f*(controlDataInst.goalLinear_v - controlDataInst.goalLinear_v_pre);
    controlDataInst.goalLinear_v_pre = controlDataInst.goalLinear_v;
    controlDataInst.goalAngular_v_pre =  controlDataInst.goalAngular_v;

    //limit control speed
    limitRange2(controlDataInst.goalAngular_v_pre,-MAX_ANGULAR_SPEED,MAX_ANGULAR_SPEED);
    limitRange2(controlDataInst.goalLinear_v_pre,MAX_LINEAR_BACK_SPEED,MAX_LINEAR_FRONT_SPEED);




    //	if(1)
    //		return;
    //	if((motor_inst.motor_parameter_Ptr->motor_type == MOTOR_JIQIN90N)&&(Switch_Flag != 1))
    //	{
    //		if((dec_flag == 1)&&(controlDataInst.goalLinear_v_pre == 0))
    //		{
    //			controlDataInst.goalLinear_v_pre = 0.1f;
    //			dec_delay_time++;
    //		}
    //		if(dec_delay_time >80)
    //		{
    //			controlDataInst.goalLinear_v_pre = 0.0f;
    //			dec_flag = 0;
    //			dec_delay_time = 0;
    //		}
    //		if((aec_flag == 1)&&(controlDataInst.goalLinear_v_pre == 0))
    //		{
    //			controlDataInst.goalLinear_v_pre = -0.1f;
    //			aec_delay_time++;
    //		}
    //		if(aec_delay_time >80)
    //		{
    //			controlDataInst.goalLinear_v_pre = 0.0f;
    //			aec_flag = 0;
    //			aec_delay_time = 0;
    //		}
    //		if((turnright_flag == 1)&&(controlDataInst.goalAngular_v_pre != 0)&&(first_turnright_flag == 0))
    //		{
    //			controlDataInst.goalAngular_v_pre = -0.3f;
    //			right_delay_time++;
    //		}
    //		if(right_delay_time >80)
    //		{
    //			first_turnright_flag = 1;
    //			controlDataInst.goalAngular_v_pre =  controlDataInst.goalAngular_v;
    //			turnright_flag = 0;
    //			right_delay_time = 0;
    //		}
    //		if((turnleft_flag == 1)&&(controlDataInst.goalAngular_v_pre != 0)&&(first_turnleft_flag == 0))
    //		{
    //			controlDataInst.goalAngular_v_pre = 0.3f;
    //			left_delay_time++;
    //		}
    //		if(left_delay_time >80)
    //		{
    //			first_turnleft_flag = 1;
    //			controlDataInst.goalAngular_v_pre =  controlDataInst.goalAngular_v;
    //			turnleft_flag = 0;
    //			left_delay_time = 0;
    //		}
    //	}

}


void Calculate_Wheel_Speed()
{
    float feedBack_control_data;

    Angular_PID.goal = controlDataInst.goalAngular_v_pre;
    Angular_PID.real = controlDataInst.presentAngular_v;

    PositionalPidControler(&Angular_PID,CTRL_T);

    if(controlDataInst.goalLinear_v_pre == 0 && controlDataInst.goalAngular_v_pre == 0)
        //if(controlDataInst.goalLinear_v_pre == 0 && controlDataInst.goalAngular_v_pre == 0 && fabs(controlDataInst.present_v[0]) <= 0.1 && fabs(controlDataInst.present_v[1]) <= 0.1)
    {
        Angular_PID.integral = 0;
        Angular_PID.pid_out  = 0;

        LeftWheel_PID.integral = 0;
        LeftWheel_PID.pid_out  = 0;
        RightWheel_PID.integral = 0;
        RightWheel_PID.pid_out  = 0;
    }

    feedBack_control_data = Angular_PID.goal * motor_inst.chassis_parameter_Ptr->wheel_separation / 2.0f;

    controlDataInst.wheelSpeed[0] = controlDataInst.goalLinear_v_pre - feedBack_control_data;
    controlDataInst.wheelSpeed[1] = controlDataInst.goalLinear_v_pre + feedBack_control_data;

    limitRange(controlDataInst.wheelSpeed[0],MAX_MOTOR_SPEED);
    limitRange(controlDataInst.wheelSpeed[1],MAX_MOTOR_SPEED);
}

void Wheel_Control()
{
    motor_inst.driver_import[LEFT_MOTOR] = 0;
    motor_inst.driver_import[RIGHT_MOTOR] = 0;

    motor_inst.set_speed[LEFT_MOTOR] = controlDataInst.wheelSpeed[LEFT_MOTOR];
    motor_inst.set_speed[RIGHT_MOTOR] = controlDataInst.wheelSpeed[RIGHT_MOTOR];

    //计算左轮PID
    LeftWheel_PID.goal = motor_inst.set_speed[LEFT_MOTOR];
    LeftWheel_PID.real = motor_inst.real_speed[LEFT_MOTOR];
    //OpenLoopController(&LeftWheel_PID);
    PositionalPidControler(&LeftWheel_PID,0.01);

    //计算右轮PID
    RightWheel_PID.goal = motor_inst.set_speed[RIGHT_MOTOR];
    RightWheel_PID.real = motor_inst.real_speed[RIGHT_MOTOR];
    //OpenLoopController(&RightWheel_PID);
    PositionalPidControler(&RightWheel_PID,0.01);


    /*
    *坡道前馈
    */
    //	static float ramp;
    //	ramp  = Ramp_FrontFeed();
    //
    //	if(ctrl_msg->lock==0)
    //	{
    //		motor_inst.driver_import[LEFT_MOTOR] +=ramp;
    //		motor_inst.driver_import[RIGHT_MOTOR] +=ramp;
    //	}
    /*end
    *坡道前馈
    */

    //驻车刹车失能开关
    if(motor_inst.lock_Disable == 1 )
    {
        motor_inst.lock = 0;
        motor_inst.BRK = 0;
        LeftWheel_PID.EN = 0;
        RightWheel_PID.EN = 0;
        Angular_PID.EN = 0;
        LeftWheel_PID.integral = 0;
        RightWheel_PID.integral = 0;
        Angular_PID.integral = 0;
    }
    else
    {
        LeftWheel_PID.EN = 1;
        RightWheel_PID.EN = 1;
        Angular_PID.EN = 1;
    }


    //驱动器不是自己闭环的，由控制板进行闭环
    if(motor_inst.driver_parameter_Ptr->motor_driver_control_type != MOTOR_DRIVER_INTERNAL_SELF_CTL)
    {
        motor_inst.driver_import[LEFT_MOTOR]  = LeftWheel_PID.pid_out;
        motor_inst.driver_import[RIGHT_MOTOR] = RightWheel_PID.pid_out;
    }

    //只有电压控制方式，需要引入前馈
    if(motor_inst.driver_parameter_Ptr->motor_driver_control_type == MOTOR_DRIVER_VOLTAGE_CTL)
    {
        motor_inst.driver_import[LEFT_MOTOR] += Linear_V_FrontFeed(LeftWheel_PID.goal,&motor_inst);
        motor_inst.driver_import[RIGHT_MOTOR] += Linear_V_FrontFeed(RightWheel_PID.goal,&motor_inst);
    }

    /*转向 并联PID*/
    motor_inst.driver_import[LEFT_MOTOR] -= Angular_PID.pid_out;
    motor_inst.driver_import[RIGHT_MOTOR] += Angular_PID.pid_out;
    /*并联PID end*/

    static float PWM_filter[2];
    static float k_1 = 0.1;

    if(motor_inst.prefilter_EN)
    {
        PWM_filter[0] = (1-k_1)*PWM_filter[0] + k_1*motor_inst.driver_import[LEFT_MOTOR];
        PWM_filter[1] = (1-k_1)*PWM_filter[1] + k_1*motor_inst.driver_import[RIGHT_MOTOR];

        motor_inst.driver_import[LEFT_MOTOR] = PWM_filter[0];
        motor_inst.driver_import[RIGHT_MOTOR] = PWM_filter[1];
    }

    /*	调试用代码 需要注释掉		*/
//		static float lock = 0;
//		static float left = 0;
//		static float right = 0;

//		motor_inst.driver_import[LEFT_MOTOR] = left;
//		motor_inst.driver_import[RIGHT_MOTOR] = right;
//		motor_inst.lock = lock;
//		motor_inst.BRK = lock;
    /*end	调试用代码 需要注释掉		*/


    writeMotorControlData(&motor_inst);
}

int Brake_Motor()
{
    static unsigned char count_brk = 51;		 //刹车延时
    static unsigned char count_loosen = 0; 		//松开刹车延时
    int brk_state;


    static float filtRealSpeed[2]= {0,0};
    filtRealSpeed[0] =filtRealSpeed[0] + 0.05f*(controlDataInst.present_v[0] - filtRealSpeed[0]);
    filtRealSpeed[1] =filtRealSpeed[1] + 0.05f*(controlDataInst.present_v[1] - filtRealSpeed[1]);

    //1 如果有控制量
    if( (fabs(controlDataInst.goalLinear_v) + fabs(controlDataInst.goalAngular_v)) >= MIN_LINEAR_SPEED)
    {
        //立刻松开刹车

        if(count_loosen > 20)//延时500ms松开刹车
        {
            motor_inst.BRK = 0;
            motor_inst.lock= 0;
        }
        else
        {
            count_loosen++;
            motor_inst.BRK = 0;//因为不需要延时故设为1
            motor_inst.lock= 0;
        }

        count_brk = 0;

        brk_state = 0;
    }
    else
    {
        //2 没有控制量了
        // 驻车

        if(count_brk>200)//4 延时500ms
        {
            //3 刹车
            motor_inst.BRK = 1;
            motor_inst.lock = 1;
        }
        else
        {
            motor_inst.BRK = 1;
            count_brk++;
            if( (fabs(filtRealSpeed[0])<MIN_LINEAR_SPEED) || (fabs(filtRealSpeed[1])<MIN_LINEAR_SPEED) )
            {
                count_brk = 254;//溢出计算器，刹车
            }

        }
        count_loosen = 0;
        brk_state = 1;
    }

    /*用于speedlimt线速度正方向给零时立刻刹车*/
    if( (!(controlDataInst.goalLinear_v<-MIN_LINEAR_SPEED)) &&
            fabs(controlDataInst.goalAngular_v)< MIN_ANGULAR_SPEED)//如果线速度为正且没有角速度
    {
        if(speed_limit[0][1]==0)//如果前进方向被禁止 这个if必须在这个驻车的逻辑里面
        {
            motor_inst.BRK = 1;
            motor_inst.lock= 1;
            brk_state = 1;
        }
    }
    /*end*用于speedlimt线速度正方向给零时立刻刹车*/

    //增加，若急停按下则停止电机
    if(motor_inst.scramFlag)
    {
        motor_inst.BRK = 1;
        motor_inst.lock= 1;
        brk_state = 1;
    }

    return brk_state;
}


void control_maintenanceMode(uint8_t cmd)
{
    if (cmd != OFF || cmd != ON )
    {
        sdlog_printf_app("inpiut error cmd = %d please inpiut 0|1 .",cmd);
        GR_LOG_ERROR("inpiut error cmd = %d please inpiut 0|1 .",cmd);
        return;
    }

    motor_inst.lock_Disable = cmd;

    //需要写入底层，否则不会生效
    writeMotorControlData(&motor_inst);
    if(motor_inst.lock_Disable == ON) //维护模式打开
    {
        sdlog_printf_app("maintenance mode is on");
        GR_LOG_INFO("maintenance mode is on .");
    }
    else
    {
        sdlog_printf_app("maintenance mode is off");
        GR_LOG_INFO("maintenance mode is off .");
    }
}
void updateControldata(ControlData_Typedef *copy_Ptr) //用于将控制层的代码拷贝出去
{
	memcpy(copy_Ptr,&controlDataInst,sizeof(ControlData_Typedef));
}
