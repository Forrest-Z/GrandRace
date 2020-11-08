#include "BLD_IO.h"

/*
	金瑞麒
	吴钦文
	2018/7/14
		优化了电机捕获,使捕获效率更高
	2018/12/10
		整个系统重构了一次
*/

#define MAX_PWM_VALUE 5000

/* 自定义代码 ----------------------------------------------------------------*/
float distance_Motor[2] = {0,0};						//定义了记录路程的变量
float speed_Motor[2] = {0,0};								//定义了记录速度的变量

unsigned int detectCounterLeft[3] = {0,0,0};					//用于检测霍尔掉线 2018/9/25
unsigned int detectCounterRight[3] = {0,0,0};


uint8_t Right_Elapsed_Counter =	0;	//用于判读TIM11捕获是否溢出
uint8_t Left_Elapsed_Counter =	0;	//用于判读TIM13捕获是否溢出

int EncoderNum[2] = {0,0};
uint32_t lastEncoderNum[2] = {0,0};

float io_wheelDiameter = 1;
float io_scaleWheel = 1;
unsigned char io_motorType;


/*********************************************************************************/

void Right_Capture(int);				//处理TIM1的中断捕获
void Right_PeriodElapsed(void);	//处理TIM1的中断溢出，目的是为了获得一个时间基准 本模块私有的

void Left_Capture(int);				//处理TIM4的中断捕获
void Left_PeriodElapsed(void);	//处理TIM4的中断溢出，目的是为了获得一个时间基准 本模块私有的

/*驱动接口
************************************************/
void BLD_IO_Init(MOTORCONTROL_Typedef *pmsg)
{
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_IT(&htim1,TIM_IT_UPDATE);

    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
    __HAL_TIM_ENABLE_IT(&htim4,TIM_IT_UPDATE);

    HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&htim13,TIM_CHANNEL_1);

    pmsg->BRK = 1;
    pmsg->EN = 1;
    pmsg->lock = 1;
    pmsg->scramFlag = 0;
    HAL_Delay(100);
    BLD_IO_Write(pmsg);
    BLD_IO_Read(pmsg);//初始化结构体

    io_wheelDiameter = pmsg->motor_parameter_Ptr->diameter_wheel;
    io_scaleWheel = pmsg->motor_parameter_Ptr->scale_wheel;
    io_motorType = pmsg->motor_parameter_Ptr->motor_type;
}

void BLD_IO_Read(MOTORCONTROL_Typedef *pmsg)
{
    pmsg->real_speed[0] = speed_Motor[0];
    pmsg->real_speed[1] = speed_Motor[1];

    pmsg->distance[0] = distance_Motor[0];
    pmsg->distance[1] = distance_Motor[1];

    if((detectCounterLeft[0]+
            detectCounterLeft[1]+
            detectCounterLeft[2])>10)
    {
        pmsg->HALLFlag[0] = 1;
        detectCounterLeft[0] = 0;
        detectCounterLeft[1] = 0;
        detectCounterLeft[2] = 0;
    }
    if((detectCounterRight[0]+
            detectCounterRight[1]+
            detectCounterRight[2])>10)
    {
        pmsg->HALLFlag[1] = 1;
        detectCounterRight[0] = 0;
        detectCounterRight[1] = 0;
        detectCounterRight[2] = 0;
    }
}

void BLD_IO_Write(MOTORCONTROL_Typedef *pmsg)
{

    EN_L_MOTOR(pmsg->EN);
    EN_R_MOTOR(pmsg->EN);
    BRK_R_MOTOR(pmsg->BRK);
    BRK_L_MOTOR(pmsg->BRK);

    setPWM_Motor(LEFT_MOTOR,-pmsg->driver_import[0]);
    setPWM_Motor(RIGHT_MOTOR,pmsg->driver_import[1]);
}



void setPWM_Motor(char ch,float pwm)//pwm为-1~1 对应 -100%~+100%
{

    float pwm_buf;

#if 1
    pwm = -pwm;
#endif

    if(pwm!= 0)
    {
        pwm_buf = fabs(pwm) + 0.066; //0.066占空比修正
    }
    else
    {
        pwm_buf = fabs(pwm);
    }
    if(pwm_buf>1)//表达超过范围
    {
        pwm_buf = 1;
    }

    EN_R_MOTOR(1);
    EN_L_MOTOR(1);
    switch(ch)
    {
    case 0:
        TIM13->CCR1=pwm_buf*MAX_PWM_VALUE;
        if(!(pwm>0))		//if(pwm>0)
        {
            DIR_L_MOTOR(0);
        }
        else
        {
            DIR_L_MOTOR(1);
        }
        break;
    case 1:
        TIM10->CCR1=pwm_buf*MAX_PWM_VALUE;
        if(!(pwm>0))		//if(pwm>0)					//改变方向
        {
            DIR_R_MOTOR(0);//宏定义
        }
        else
        {
            DIR_R_MOTOR(1);
        }
        break;

    default:
        break;
    }

}

float getDistance_Motor(char ch)
{
    static unsigned char count = 0;//wqw 2018/8/9 用于电机驱动初始化延时，刚开机的1s钟不采集电机,否则会有微小的数据累加
    if(count<100)
    {
        count++;
        distance_Motor[ch - 1] = 0;
        return 0;
    }

    if(ch<=0)		//防止指针寻址出错
        return 0;
    if(ch>sizeof(distance_Motor)/sizeof(float))
        return 0;

    return distance_Motor[ch - 1];
}

void clearDistance_Motor(char ch)
{
    distance_Motor[ch - 1] = 0;
}

void BLD_IO_LeftSpeedCaptureCallback(TIM_HandleTypeDef *htim)
{
    static int dirLeft = 0; //左轮的方向
    if(htim == &htim4)
    {
        switch(htim->Channel)
        {
        case HAL_TIM_ACTIVE_CHANNEL_1:
            if(HAL_GPIO_ReadPin(H_L_V_GPIO_Port,H_L_V_Pin))
            {
                dirLeft = -1;
            }
            else {
                dirLeft = 1;
            }
            Left_Capture(dirLeft);
            detectCounterLeft[0] = 0;
            detectCounterLeft[1] ++;
            detectCounterLeft[2] ++;
            HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
            break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
            if(HAL_GPIO_ReadPin(H_L_W_GPIO_Port,H_L_W_Pin))
            {
                dirLeft = -1;
            }
            else {
                dirLeft = 1;
            }
            Left_Capture(dirLeft);
            detectCounterLeft[0] ++;
            detectCounterLeft[1] = 0;
            detectCounterLeft[2] ++;
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
            HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_2);
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
            break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
            if(HAL_GPIO_ReadPin(H_L_U_GPIO_Port,H_L_U_Pin))
            {
                dirLeft = -1;
            }
            else {
                dirLeft = 1;
            }
            Left_Capture(dirLeft);
            detectCounterLeft[0] ++;
            detectCounterLeft[1] ++;
            detectCounterLeft[2] = 0;
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
            HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
            HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_3);
            break;
        default:
            break;
        }

    }

}

int debug_cc1;
int debug_cc2;
int debug_cc3;

void BLD_IO_RightSpeedCaptureCallback(TIM_HandleTypeDef *htim)
{
    static int dirRight = 0; //右轮的方向

    if(htim == &htim1)
    {
        switch(htim->Channel)
        {
        case HAL_TIM_ACTIVE_CHANNEL_1:
            if(HAL_GPIO_ReadPin(H_R_V_GPIO_Port,H_R_V_Pin))
            {
                dirRight = -1;
                debug_cc1 = -1;
            }
            else {
                dirRight = 1;
                debug_cc1 = 1;
            }
            detectCounterRight[0] = 0;
            detectCounterRight[1] ++;
            detectCounterRight[2] ++;
            HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_1);
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_3);
            Right_Capture(dirRight);

            break;
        case HAL_TIM_ACTIVE_CHANNEL_2:
            if(HAL_GPIO_ReadPin(H_R_W_GPIO_Port,H_R_W_Pin))
            {
                dirRight = -1;
                debug_cc2 = -1;
            }
            else {
                dirRight = 1;
                debug_cc2 = 1;
            }
            detectCounterRight[0] ++;
            detectCounterRight[1] = 0;
            detectCounterRight[2] ++;
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
            HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_3);
            Right_Capture(dirRight);

            break;
        case HAL_TIM_ACTIVE_CHANNEL_3:
            if(HAL_GPIO_ReadPin(H_R_U_GPIO_Port,H_R_U_Pin))
            {
                dirRight = -1;
                debug_cc3 = -1;
            }
            else {
                dirRight = 1;
                debug_cc3 = 1;
            }
            detectCounterRight[0] ++;
            detectCounterRight[1] ++;
            detectCounterRight[2] = 0;
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
            HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);
            HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_3);
            Right_Capture(dirRight);

            break;
        default:
            break;

        }

    }

}

void BLD_IO_LeftSpeedPeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim4)
    {
        Left_PeriodElapsed();
    }
}
void BLD_IO_RightSpeedPeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim1)
    {
        Right_PeriodElapsed();
    }
}

void Right_Capture(int dir)
{
//速度部分
    static uint32_t Capture_buf[2];//存两次捕获数据


    if( io_motorType == MOTOR_ORIGINAL)
    {
        dir = -dir;
    }



    Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_1);
    /***************************************/
    switch(htim1.Channel)
    {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_1);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_2);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim1,TIM_CHANNEL_3);
        break;
    default:
        break;
    }

    /***************************************/

    if(Right_Elapsed_Counter != 2) //捕获没有溢出，可靠
    {
        uint32_t buf;//用于溢出修正

        //增加了计数器的长度
        buf = Right_Elapsed_Counter * htim1.Init.Period + Capture_buf[0] - Capture_buf[1];

        //速度 = 轮周长/分度/捕获的时间 单位M/s

        //极值滤波
        float speed_buf = (float)dir * io_wheelDiameter*pi/io_scaleWheel/( buf*(2.0f/10000.0f) )*2.0f;//修正量*2.0f
        if(fabs(speed_buf)>3.0f)
            return;
        speed_Motor[1] = speed_buf;
    }
    Capture_buf[1] = Capture_buf[0];//无论是否溢出，前一次的数据都是有效的

    Right_Elapsed_Counter = 0;//每次捕获会清除它

    /***********朱吉林的代码**********/
    if(dir>0)
    {
        EncoderNum[1]++;
    }
    else
    {
        EncoderNum[1]--;
    }

    if(EncoderNum[1] > 500000 || EncoderNum[1] < -500000)
    {
        EncoderNum[1] = 0;
    }
    /***********朱吉林的代码**********/

//路程处理部分
    //路程 = 轮周长/分度 单位M
    distance_Motor[1] += dir * io_wheelDiameter*pi/io_scaleWheel;


//获得上一次的捕获时间 //采样周期为0.1ms

}
void Right_PeriodElapsed()
{

    Right_Elapsed_Counter++;//每次捕获会清除它
    if(Right_Elapsed_Counter >= 2)//溢出两次都没有捕获到传感器，判读速度为0
    {
        Right_Elapsed_Counter = 2;
        speed_Motor[1] = 0;	//右轮
    }
}

void Left_Capture(int dir)
{
//速度部分
    static uint32_t Capture_buf[2];//存两次捕获数据


    if (io_motorType == MOTOR_ORIGINAL)
    {
        dir = -dir;
    }


    if((io_motorType == MOTOR_ZHAOYANG)
            ||(io_motorType == MOTOR_JIQIN90N)
            ||(io_motorType == MOTOR_LIANYI60N)
						||(io_motorType == MOTOR_LIANYI_NEW))
    {
        dir = -dir;
    }



    Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_1);
    /***************************************/
    switch(htim4.Channel)
    {
    case HAL_TIM_ACTIVE_CHANNEL_1:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_1);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_2:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_2);
        break;
    case HAL_TIM_ACTIVE_CHANNEL_3:
        Capture_buf[0] = HAL_TIM_ReadCapturedValue(&htim4,TIM_CHANNEL_3);
        break;
    default:
        break;
    }

    /***************************************/

    if(Left_Elapsed_Counter != 2) //捕获没有溢出，可靠
    {
        uint32_t buf;//用于溢出修正

        buf = Left_Elapsed_Counter * htim4.Init.Period +  Capture_buf[0] - Capture_buf[1];

        //速度 = 轮周长/分度/捕获的时间 单位M/s

        //极值滤波
        float speed_buf = (float)dir * io_wheelDiameter*pi/io_scaleWheel/( buf*(2.0f/10000.0f) )*2.0f;//修正量*2.0f
        if(fabs(speed_buf)>3)
            return;

        speed_Motor[0] = speed_buf;
    }


    Capture_buf[1] = Capture_buf[0];//无论是否溢出，前一次的数据都是有效的

    Left_Elapsed_Counter = 0;//每次捕获会清除它

    /***********朱吉林的代码**********/
    if(dir>0)
    {
        EncoderNum[0]++;
    }
    else
    {
        EncoderNum[0]--;
    }

    if(EncoderNum[0] > 500000 || EncoderNum[0] < -500000)
    {
        EncoderNum[0] = 0;
    }

    /***********朱吉林的代码**********/

//路程处理部分
    //路程 = 轮周长/分度 单位M
    distance_Motor[0] += dir * io_wheelDiameter*pi/io_scaleWheel;


//获得上一次的捕获时间 //采样周期为0.1ms

}
void Left_PeriodElapsed()
{
    Left_Elapsed_Counter++;//每次捕获会清除它
    if(Left_Elapsed_Counter>=2)//溢出两次都没有捕获到传感器，判读速度为0
    {
        Left_Elapsed_Counter = 2;
        speed_Motor[0] = 0;	//左轮
    }
}

void BLD_IO_Stop(void)
{
    MOTORCONTROL_Typedef motor_stop;
    motor_stop.BRK = 1;
    motor_stop.lock = 1;
    motor_stop.EN = 0;
    motor_stop.driver_import[0] = 0;
    motor_stop.driver_import[1] = 0;
    BLD_IO_Write(&motor_stop);
}



#if 0
uint8_t brake_Motor(float *speedGoalLeft,float *speedGoalRight)
{
    static unsigned char count;
    if(speedGoalLeft < 0.1 && speedGoalLeft > -0.1)//电机速度小于0.1m/s
    {
        speedGoalLeft = 0;
    }
    if(speedGoalRight < 0.1 && speedGoalRight > -0.1)
    {
        speedGoalLeft = 0;
    }

    if( fabs(speedGoalLeft)+fabs(speedGoalRight) )
    {
        //松开刹车
        EN_Lock_MOTOR(0);

        if(count>100)
        {
            count--;
            return 1;
        }
        count = 0;
        return 0;

    } else
    {
        //刹车
        if( fabs(speed_Motor[0])+fabs(speed_Motor[1]) && count<50 )//速度不为零且没有到0.5s
        {
            count ++;
            return 0;//返回0表示没有刹车
        }

        count = 120;
        EN_Lock_MOTOR(1);
        setPWM_Motor(1,0);
        setPWM_Motor(2,0);
        return 1;
    }
}
#endif


/*****************************END OF FILE*********************************/
