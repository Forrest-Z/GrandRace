
#include "idea.h"
#include "math.h"
#include "imu.h"
#include "Clock.h"
/*****************************************************************************/
//单位m/s^2
float gravityAtAxis[3]; 	//重力映射到各个轴的风量，依赖于四元数的正确性
float ACC_NoGravity[3];		//移出重力后的各个轴的加速度

float yaw;
float roll;
float pitch;

OrientationData q_x_axis;//旋转后的轴在绝对坐标系的映射
OrientationData q_y_axis;
OrientationData q_z_axis;

ImuPara_TypeDef imuSensor;
/*****************************************************************************/

OrientationData OrientationMUL(OrientationData,OrientationData);

/*****************************************************************************/

void idea_main()
{
    //calGravityToAxis();
}

float getTs_idea(float *lastStamp)
{
	
	float newStamp;
	float ts;
	newStamp = ( (float)getSystemTick() ) / 1000.0f;
	ts = newStamp - *lastStamp;
	*lastStamp = newStamp;	
	return ts;//返回最近的两次调用的时间间隔 (s)
}

void sendWheelSpeed()
{
    float angle_velocity;
    static float angle;

    static unsigned int count = 0;

    getImuData(&imuSensor);
    angle_velocity = imuSensor.Real_Gyro[2];

    if(angle>360)
    {
        angle -= 360;
    }
    if(angle<0)
    {
        angle += 360;
    }

    count++;
    if(count%10!=0) {
        return;
    }
    printf("\n");
    printf("wheel rmp:        %f\n",angle_velocity/360.0f);
    //printf("wheelSpeed:       %f\n",angle_velocity/180*pi*motor_parameter_.DIAMETER_WHEEL_/2);

}


float getSinWave()
{
    float f = 2.0f;//2hz
    const float ts = 0.01f;
    static float t = 0;
    float w;
    t += ts;
    if(t>1/f)
        t -= 1/f;
    w = 2.0f*pi*f*t;
    return sin(w);
}


//返回X轴
OrientationData *get_axisOfx()
{
	return &q_x_axis;
}
void calGravityToAxis()
{
    const float x_axis[3] = {1,0,0};//车体x轴关于绝对坐标系表达(未旋转)
    const float y_axis[3] = {0,1,0};//车体x轴关于绝对坐标系表达(未旋转)
    const float z_axis[3] = {0,0,1};//车体x轴关于绝对坐标系表达(未旋转)

    OrientationData q;
    OrientationData q_c;//四元数的共轭
    OrientationData q_buf;//中间变量
    float *q_ptr;

    getImuData(&imuSensor);
    q_ptr = imuSensor.orientation;//获得四元数

    q.q0 = q_ptr[0];
    q.q1 = q_ptr[1];
    q.q2 = q_ptr[2];
    q.q3 = q_ptr[3];

    q_c.q0 = q.q0;
    q_c.q1 = -q.q1;
    q_c.q2 = -q.q2;
    q_c.q3 = -q.q3;

    q_x_axis.q0 = 0;
    q_x_axis.q1 = x_axis[0];
    q_x_axis.q2 = x_axis[1];
    q_x_axis.q3 = x_axis[2];

    q_y_axis.q0 = 0;
    q_y_axis.q1 = y_axis[0];
    q_y_axis.q2 = y_axis[1];
    q_y_axis.q3 = y_axis[2];

    q_z_axis.q0 = 0;
    q_z_axis.q1 = z_axis[0];
    q_z_axis.q2 = z_axis[1];
    q_z_axis.q3 = z_axis[2];


    q_buf = OrientationMUL(q,q_x_axis);
    q_x_axis = OrientationMUL(q_buf,q_c);//获得旋转后的值
    q_buf = OrientationMUL(q,q_y_axis);
    q_y_axis = OrientationMUL(q_buf,q_c);//获得旋转后的值
    q_buf = OrientationMUL(q,q_z_axis);
    q_z_axis = OrientationMUL(q_buf,q_c);//获得旋转后的值

    gravityAtAxis[0] =q_x_axis.q3 * 9.8f;
    gravityAtAxis[1] =q_y_axis.q3 * 9.8f;
    gravityAtAxis[2] =q_z_axis.q3 * 9.8f;
    ACC_NoGravity[0] = imuSensor.ACC_FilterData[0] * 0.000598f -gravityAtAxis[0];//去掉重力在x轴上的分量
    ACC_NoGravity[1] = imuSensor.ACC_FilterData[1] * 0.000598f -gravityAtAxis[1];//去掉重力在y轴上的分量
    ACC_NoGravity[2] = imuSensor.ACC_FilterData[2] * 0.000598f -gravityAtAxis[2];//去掉重力在z轴上的分量

    yaw = atan2(q_x_axis.q2,q_x_axis.q1)/pi*180.0f;
    if(yaw < 0.0f)
        yaw += 360.0f;
    yaw = 360.0f - yaw;
    roll = asinf(q_y_axis.q3)/pi*180.0f;
    pitch = asinf(q_x_axis.q3)/pi*180.0f;

//	static DebugMsg debugMsg;
    static unsigned int count = 0;
    count++;
    if(count%10==0) {

//	debugMsg.upload[0] = (int)yaw;
//	debugMsg.upload[1] = (int)roll;
//	debugMsg.upload[2] = (int)pitch;
//
//	debugMsg.publish();
    }
}


/*
*功能：四元数乘法
*/
OrientationData OrientationMUL(OrientationData qa,OrientationData qb)
{

    OrientationData q;

    q.q0 = qa.q0*qb.q0-qa.q1*qb.q1-qa.q2*qb.q2-qa.q3*qb.q3;
    q.q1 = qa.q1*qb.q0+qa.q0*qb.q1-qa.q3*qb.q2+qa.q2*qb.q3;
    q.q2 = qa.q2*qb.q0+qa.q3*qb.q1+qa.q0*qb.q2-qa.q1*qb.q3;
    q.q3 = qa.q3*qb.q0-qa.q2*qb.q1+qa.q1*qb.q2+qa.q0*qb.q3;

    return q;


}

/*end********************************************************************/
