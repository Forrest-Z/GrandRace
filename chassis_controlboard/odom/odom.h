#ifndef _ODOM_H_
#define _ODOM_H_

#pragma pack(1)

#define WHEEL_NUM                        2
#define LEFT                             0
#define RIGHT                            1

typedef struct
{
    unsigned char init_encoder;
    float last_diff_tick[2];
    float last_rad[2];
    float diff_time;						//(s) 微分时间
    float odom_pose[3];
    float odom_vel[3];
    float  last_velocity[2];
    float  mileage[2];
    float  total_mileage;			// km
		float diff_distance;			//与起点的距离 (m)
} Odom_TypeDef;



void readOdomDevice(void);
void getOdomData(Odom_TypeDef *pmsg);


#endif

