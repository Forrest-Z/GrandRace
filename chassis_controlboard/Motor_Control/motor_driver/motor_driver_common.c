
#include "motor_driver_common.h"


void Motor_Driver_Common_Write(MOTORCONTROL_Typedef *pmsg)
{

    static unsigned char lock_count = 0;//用于避免继电器频繁切换

    if(SCRAM_STATE)
        pmsg->scramFlag = 1;
    else
        pmsg->scramFlag = 0;


    if(pmsg->lock)
    {   //刹车需要等0.5s
        lock_count++;
        if(lock_count>10)
        {
            lock_count = 50;
            EN_Lock_MOTOR(pmsg->lock);
        }
    }
    else
    {   //松开刹车是立刻的
        lock_count = 0;
        EN_Lock_MOTOR(pmsg->lock);
    }
}

void Motor_Driver_Common_Read(MOTORCONTROL_Typedef *pmsg)
{

}

