#include "odom.h"
#include "imu.h"
#include "motor_driver.h"
#include "motor_driver.h"
#include "Clock.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "idea.h"
//#include "monitor.h"

Odom_TypeDef ros_odom;

#if 1
float* getOrientation(ImuPara_TypeDef pmsg)
{
    static float orientation[4];

    orientation[0] = pmsg.orientation[0];
    orientation[1] = pmsg.orientation[1];
    orientation[2] = pmsg.orientation[2];
    orientation[3] = pmsg.orientation[3];

    return orientation;
}
#endif

void getEncoderInfo(MOTORCONTROL_Typedef *motor_msg, Odom_TypeDef *odom_info)
{
    float left_tick,right_tick;
    float current_tick = 0;
    static float last_tick[WHEEL_NUM] = {0.0, 0.0};
    unsigned char index;

    left_tick  = motor_msg->distance[0];
    right_tick = motor_msg->distance[1];

    if(odom_info->init_encoder)
    {
        for (index = 0; index < WHEEL_NUM; index++)
        {
            odom_info->last_diff_tick[index] = 0.0;
            last_tick[index] = 0.0;
            odom_info->last_rad[index]       = 0.0;

            odom_info->last_velocity[index]  = 0.0;
        }

        last_tick[LEFT]  = -left_tick;
        last_tick[RIGHT] = -right_tick;

        odom_info->init_encoder = 0;
        return;
    }

    current_tick = left_tick;

    odom_info->last_diff_tick[LEFT] = current_tick - last_tick[LEFT];


    last_tick[LEFT] = current_tick;
    odom_info->last_rad[LEFT] += odom_info->last_diff_tick[LEFT];

    current_tick = right_tick;

    odom_info->last_diff_tick[RIGHT] = current_tick - last_tick[RIGHT];

    last_tick[RIGHT] = current_tick;
    odom_info->last_rad[RIGHT]  += odom_info->last_diff_tick[RIGHT];
}

/*******************************************************************************
* Calculate the odometry
*******************************************************************************/
//float Control_wData;

unsigned char calcOdometry_1(Odom_TypeDef *odom_info)//在这个地方更新了odom的数组
{
    float* orientation;
    float wheel_l, wheel_r;      // rotation value of wheel [rad]
    float delta_s, theta, delta_theta;
    static float last_theta = 0.0;
//  float v, w;                  // v = translational velocity [m/s], w = rotational velocity [rad/s]
    float step_time;
    static ImuPara_TypeDef imu_q;
    wheel_l = wheel_r = 0.0;
    delta_s = delta_theta = theta = 0.0;
//  v = w = 0.0;
    step_time = 0.0;

    step_time = odom_info->diff_time;

    if (step_time == 0)
        return 0;

    wheel_l = odom_info->last_diff_tick[LEFT];
    wheel_r = odom_info->last_diff_tick[RIGHT];

    if (isnan(wheel_l))
        wheel_l = 0.0;

    if (isnan(wheel_r))
        wheel_r = 0.0;

    delta_s     = (wheel_r + wheel_l) / 2.0f;
    // theta = WHEEL_RADIUS * (wheel_r - wheel_l) / WHEEL_SEPARATION;
    getImuData(&imu_q);
    orientation = getOrientation(imu_q); //imu_q.orientation;	//getOrientation(imu_q);//-1
    theta       = atan2f(orientation[1]*orientation[2] + orientation[0]*orientation[3],
                         0.5f - orientation[2]*orientation[2] - orientation[3]*orientation[3]);

    delta_theta = theta - last_theta;

//  v = delta_s / step_time;
//  w = delta_theta / step_time;

    odom_info->last_velocity[LEFT]  = wheel_l / step_time;
    odom_info->last_velocity[RIGHT] = wheel_r / step_time;

    // compute odometric pose
    odom_info->odom_pose[0] += delta_s * cos(odom_info->odom_pose[2] + (delta_theta / 2.0f));
    odom_info->odom_pose[1] += delta_s * sin(odom_info->odom_pose[2] + (delta_theta / 2.0f));
    odom_info->odom_pose[2] += delta_theta;


    // compute odometric instantaneouse velocity
    odom_info->odom_vel[2] = odom_info->odom_pose[2] + (delta_theta / 2.0f);
    odom_info->odom_vel[0] = cos(odom_info->odom_vel[2]);//速度分量的单位化投影
    odom_info->odom_vel[1] = sin(odom_info->odom_vel[2]);//
    /*end******************************************************/

    last_theta = theta;

    return 1;
}

//重写odom的计算
unsigned char calcOdometry(Odom_TypeDef *odom_info)
{    
    float delta_s;
		float delta_v;    
    delta_s  = 0.0;
		delta_v = 0.0;

    if (odom_info->diff_time == 0)
        return 0;

    if (isnan(odom_info->last_diff_tick[LEFT]))
        odom_info->last_diff_tick[LEFT] = 0.0;

    if (isnan(odom_info->last_diff_tick[RIGHT]))
        odom_info->last_diff_tick[RIGHT] = 0.0;
			
    delta_s = (odom_info->last_diff_tick[LEFT] + odom_info->last_diff_tick[RIGHT]) / 2.0f; //这是对改变距离的采样
		delta_v = delta_s/odom_info->diff_time;
		OrientationData *axis_x_ptr;
		axis_x_ptr = get_axisOfx();
		
		odom_info->total_mileage += delta_s;
    // compute odometric pose
    odom_info->odom_pose[0] += delta_s * axis_x_ptr->q1;
    odom_info->odom_pose[1] += delta_s * axis_x_ptr->q2;
	//	odom_info->odom_pose[2] += delta_s * axis_x_ptr->q3;    

		odom_info->diff_distance = sqrt(odom_info->odom_pose[0]*odom_info->odom_pose[0]+odom_info->odom_pose[1]*odom_info->odom_pose[1]);
    // compute odometric instantaneouse velocity    
    odom_info->odom_vel[0] = delta_v * axis_x_ptr->q1;
    odom_info->odom_vel[1] = delta_v * axis_x_ptr->q2;
		//odom_info->odom_vel[2] = odom_info->odom_pose[2] + (delta_theta / 2.0f);
    /*end******************************************************/

    return 1;
}

#if 0
void calculationChassisTotalMileage(Odom_TypeDef *odom_info)
{
    static float s_mileage = 0;
    float real_mileage = 0;
    static float total_mileage = 0;
    char buffer[24] = {0};
    char *str = NULL;
    static uint8_t init_ok = 0;

    if(init_ok == 0)
    {
        str = read_env_app("total_mileage");

        if(str == NULL)
            total_mileage = 0;
        else
            total_mileage = atof(str);

        init_ok = 1;
    }

    real_mileage = (odom_info->mileage[LEFT] + odom_info->mileage[RIGHT]) / 2;

#if 1
    if(real_mileage > (s_mileage + 100) && (getParkingState()))			// 停车时才存储该里程
//	if(real_mileage > (s_mileage + 100))
    {
        total_mileage += (real_mileage - s_mileage) / 1000.0f;
        sprintf(buffer, "%f", total_mileage);
        if(EF_NO_ERR == write_env_app("total_mileage", buffer))
        {
            s_mileage = real_mileage;
        }
        else
        {
            total_mileage -= (real_mileage - s_mileage) / 1000.0f;
        }
    }
#endif

    //odom_info->total_mileage = total_mileage;
    odom_info->total_mileage = total_mileage + (real_mileage - s_mileage) / 1000.0f;
}
#endif

void calculationChassisMileageData(Odom_TypeDef *odom_info)
{
    odom_info->mileage[LEFT] += fabs(odom_info->last_diff_tick[LEFT]);
    odom_info->mileage[RIGHT] += fabs(odom_info->last_diff_tick[RIGHT]);
//	calculationChassisTotalMileage(odom_info);
}

void readOdomDevice(void)			// 增加返回码
{

    static MOTORCONTROL_Typedef motor_arg;
    static Odom_TypeDef odom_info_cal;
		static float lastTime;

    updateMotorData(&motor_arg);
	
		odom_info_cal.diff_time = getTs_idea(&lastTime);
	
    getEncoderInfo(&motor_arg,&odom_info_cal);  //执行了一次底层单轮距离编码值的拷贝
    calculationChassisMileageData(&odom_info_cal);
    calcOdometry(&odom_info_cal);
    memcpy(&ros_odom, &odom_info_cal, sizeof(Odom_TypeDef));
}

void getOdomData(Odom_TypeDef *pmsg)
{
    memcpy(pmsg, &ros_odom, sizeof(Odom_TypeDef));
}
