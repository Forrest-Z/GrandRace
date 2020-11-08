#ifndef _IDEA_H
#define _IDEA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "control_config.h"
#include "imu.h"

//这个文件中的函数用于验证自己的想法
#pragma pack(1)

//限制-x <y< x
#define limitRange(y,x) y>x?(y=x):(y<-x?(y=-x):(y=y))

//限制x1 <y< x2
#define limitRange2(y,x1,x2) y>x2?(y=x2):(y<x1?(y=x1):(y=y))

#define Bit(z,y,x) x?(z|=y):(z&=~(y))


//数据结构定义区
typedef struct {
    double q0;
    double q1;
    double q2;
    double q3;
} OrientationData;

//变量定义区
extern float ACC_NoGravity[3];

extern OrientationData q_x_axis;
extern float yaw;
extern float roll;
extern float pitch;

//函数定义区

void idea_main(void);
void sendWheelSpeed(void);

OrientationData *get_axisOfx(void);		//返回X轴
void calGravityToAxis(void);//得到各个轴的重力加速度分量

float getTs_idea(float *lastStamp);

#ifdef __cplusplus
}
#endif

#endif
