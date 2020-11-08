#include "active_safety.h"
#include "math.h"

#ifdef __SAFETY_USER

uint32_t ultrasonicData[ULTRASONIC_NUM];
Ultrasonic_TypeDef raw_ultrasonic;


#define front_safeStatus (ultrasonicData[0] < SAFE_DISTANCE_FRONT || \
													ultrasonicData[1] < SAFE_DISTANCE_FRONT || \
													ultrasonicData[2] < SAFE_DISTANCE_FRONT )

#define back_safeStatus (ultrasonicData[6] < SAFE_DISTANCE_BACK || \
												 ultrasonicData[7] < SAFE_DISTANCE_BACK || \
												 ultrasonicData[8] < SAFE_DISTANCE_BACK)

#define left_safeStatus (ultrasonicData[9] < SAFE_DISTANCE_LEFT || \
                         ultrasonicData[10] < SAFE_DISTANCE_LEFT || \
												 ultrasonicData[11] < SAFE_DISTANCE_LEFT)

#define right_safeStatus (ultrasonicData[3] < SAFE_DISTANCE_RIGHT || \
                          ultrasonicData[4] < SAFE_DISTANCE_RIGHT || \
												  ultrasonicData[5] < SAFE_DISTANCE_RIGHT)

#define front_breakStatus (ultrasonicData[0] < BRAKE_DISTANCE_FRONT || \
													 ultrasonicData[1] < BRAKE_DISTANCE_FRONT || \
													 ultrasonicData[2] < BRAKE_DISTANCE_FRONT )

#define back_breakStatus (ultrasonicData[6] < BRAKE_DISTANCE_BACK || \
                          ultrasonicData[7] < BRAKE_DISTANCE_BACK || \
												  ultrasonicData[8] < BRAKE_DISTANCE_BACK)

#define left_breakStatus (ultrasonicData[9] < BRAKE_DISTANCE_LEFT || \
                          ultrasonicData[10] < BRAKE_DISTANCE_LEFT || \
												  ultrasonicData[11] < BRAKE_DISTANCE_LEFT)

#define right_breakStatus (ultrasonicData[3] < BRAKE_DISTANCE_RIGHT || \
                           ultrasonicData[4] < BRAKE_DISTANCE_RIGHT || \
												   ultrasonicData[5] < BRAKE_DISTANCE_RIGHT)

void active_safetyLogic(int *status,unsigned char safe_flag,unsigned char break_flag);

void Read_Ultrasonicdata(float *inputGoalControlSpeed)
{
    int i;
    //Ultrasonic_TypeDef raw_ultrasonic;

    //根据控制方向确定需要的超声波
    if(inputGoalControlSpeed[0]>0.1f)
    {
        raw_ultrasonic.orientation |= 0x01;
        raw_ultrasonic.orientation &= ~0x04;
    } else if(inputGoalControlSpeed[0]<-0.1f)
    {
        raw_ultrasonic.orientation |= 0x04;
        raw_ultrasonic.orientation &= ~0x01;
    } else
    {
        raw_ultrasonic.orientation &= ~0x05;
    }

    if(inputGoalControlSpeed[1]>0.1f)
    {
        raw_ultrasonic.orientation |= 0x08;
        raw_ultrasonic.orientation &= ~0x02;
    } else if(inputGoalControlSpeed[1]<-0.1f)
    {
        raw_ultrasonic.orientation |= 0x02;
        raw_ultrasonic.orientation &= ~0x08;
    } else
    {
        raw_ultrasonic.orientation &= ~0x0A;
    }

    if( (fabs(inputGoalControlSpeed[0])+ \
            fabs(inputGoalControlSpeed[1]) ) <0.1f)
    {
        raw_ultrasonic.orientation = 0x0f;
    }
    //raw_ultrasonic.orientation =0x0f;		//给算法组测试用
//	raw_ultrasonic.orientation = 0x01;
//	Driver_Ultrasonic_Config(&raw_ultrasonic.orientation);
    //根据控制方向确定需要的超声波end
//    Driver_Ultrasonic_Update(&raw_ultrasonic);
    for(i=0; i<ULTRASONIC_NUM; i++)
    {
        ultrasonicData[i] = raw_ultrasonic.distance[i];
    }
}

/*
  0 1 2
8       3
7    	  4
  6   5
*/

/*
  0 1 2
11      3
10   	  4
9       5
  8 7 6
*/

int obstacle_flag[4] = {0,0,0,0};//前后左右 对应方向的安全等级 1:限速 2：停止
void active_safetyHanlder(float *inputGoalControlSpeed,float *outputActiveSafetySpeed)
{

    Read_Ultrasonicdata(inputGoalControlSpeed);//关于加速采样率，只需要在获取数据的时候依据期望操作就好

    //同侧方向的超声波安全状态，刷新安全等级 1:限速 2：停止
    active_safetyLogic(&obstacle_flag[0],front_safeStatus,front_breakStatus);
    active_safetyLogic(&obstacle_flag[1],back_safeStatus,back_breakStatus);
    active_safetyLogic(&obstacle_flag[2],left_safeStatus,left_breakStatus);
    active_safetyLogic(&obstacle_flag[3],right_safeStatus,right_breakStatus);

    /*****************************************************************/
    //根据安全等级确定输出行为
    outputActiveSafetySpeed[0] = inputGoalControlSpeed[0];
    outputActiveSafetySpeed[1] = inputGoalControlSpeed[1];
    if(inputGoalControlSpeed[0]>0.05f)
    {
        switch(obstacle_flag[0])
        {
        case 2:
            outputActiveSafetySpeed[0] = 0;
            break;
        case 1:
            if(inputGoalControlSpeed[0]>1)
                outputActiveSafetySpeed[0] = 1;
            break;
        }
    } else
    {
        switch(obstacle_flag[1])
        {
        case 2:
            outputActiveSafetySpeed[0] = 0;
            break;
        case 1:
            if(inputGoalControlSpeed[0]<-1)
                outputActiveSafetySpeed[0] = -1;
            break;
        }
    }

    if(inputGoalControlSpeed[1]>0.05f)
    {
        switch(obstacle_flag[2])
        {
        case 2:
            outputActiveSafetySpeed[1] = 0;
            break;
        case 1:
            if(inputGoalControlSpeed[1]>1)
                outputActiveSafetySpeed[1] = 1;
            break;
        }
    } else
    {
        switch(obstacle_flag[3])
        {
        case 2:
            outputActiveSafetySpeed[1] = 0;
            break;
        case 1:
            if(inputGoalControlSpeed[1]<-1)
                outputActiveSafetySpeed[1] = -1;
            break;
        }
    }
    //根据安全等级确定输出行为end

}
void active_safetyLogic(int *status,unsigned char safe_flag,unsigned char break_flag)
{
    switch(*status)
    {
        static unsigned int count = 0;
    case 0:
        if(safe_flag)
        {
            *status = 1;
        }
        break;
    case 1:
        if(break_flag)
        {
            *status = 2;
        } else
        {
            if(!safe_flag)
            {
                count++;
                if(count>150)//延迟1.5s
                {
                    *status = 0;
                    count = 0;
                }
            }
        }
        break;
    case 2:
        if(!break_flag)
        {
            count++;
            if(count>150)//延迟1.5s
            {
                *status = 1;
                count = 0;
            }
        }
        break;
    default :
        break;
    }

}

#endif

