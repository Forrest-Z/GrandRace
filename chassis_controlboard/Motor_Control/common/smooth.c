#include "smooth.h"
#include "idea.h"
//float data_debug = 0;
uint8_t dec_flag = 0;
uint8_t aec_flag = 0;
uint8_t acc_flag = 0;
uint8_t turnleft_flag = 0;
uint8_t turnright_flag = 0;
uint8_t first_turnleft_flag = 0;
uint8_t first_turnright_flag = 0;
void LineVelocitySmooth(ControlData_Typedef *controlData,float diff_time)
{
    float calAcc_linear;
    float ACC_data,Dec_data;

    calAcc_linear =  (controlData->goalLinear_v - controlData->goalLinear_v_pre) / diff_time;

    if((controlData->present_v[0] + controlData->present_v[1])/2>=0)
    {
        ACC_data = controlData->Max_front_acc;
        Dec_data = controlData->Max_front_dec;
    }
    else
    {
        Dec_data = controlData->Max_back_acc;
        ACC_data = controlData->Max_back_dec;
    }

    if(calAcc_linear > ACC_data)
    {
        controlData->goalLinear_v_pre += ACC_data * diff_time;
        //controlData->goalLinear_v_pre += 0.001f * diff_time;
        if((controlData->goalLinear_v)>=0)
        {
            acc_flag = 1;
        }
    }
    else if(calAcc_linear < -Dec_data)
    {
        controlData->goalLinear_v_pre -= Dec_data * diff_time;
        if((controlData->goalLinear_v)>=0)
        {
            dec_flag = 1;
        }
        else if((controlData->goalLinear_v)<=0)
        {
            aec_flag = 1;
        }
    }
    else
    {
        controlData->goalLinear_v_pre = controlData->goalLinear_v;
    }
    controlData->goalLinear_v_pre = controlData->goalLinear_v;

}
float angulat_test = 0;
void AngularSpeedSmooth(ControlData_Typedef *controlData,float diff_time)
{   //角加速度

    float calAcc_Angular;
    calAcc_Angular =  (controlData->goalAngular_v - controlData->goalAngular_v_pre) / diff_time;
    angulat_test = calAcc_Angular;
    if((first_turnleft_flag != 1)&&((controlData->goalAngular_v)>0.5f))
    {
        turnleft_flag = 1;
    }
    if((first_turnright_flag != 1)&&((controlData->goalAngular_v)<-0.5f))
    {
        turnright_flag = 1;
    }
    if((controlData->goalAngular_v) == 0.0f)
    {
        first_turnleft_flag = 0;
        first_turnright_flag = 0;
    }
    if(calAcc_Angular > controlData->Max_angular_acc)
    {
        controlData->goalAngular_v_pre += controlData->Max_angular_acc * diff_time;
    }
    else if(calAcc_Angular < -controlData->Max_angular_dec)
    {
        controlData->goalAngular_v_pre -= controlData->Max_angular_dec * diff_time;
    }
    else
    {
        controlData->goalAngular_v_pre =  controlData->goalAngular_v;
    }
    controlData->goalAngular_v_pre =  controlData->goalAngular_v;
}

//Smoothness ratio
#define SMOOTHNESS             0.25f
#define RATIO                  95.5f

float Cal_e_data(float x)   //  1/(1 + e^x)
{
    float value = 0;
    value = 1.0/(1.0 + exp(x));
    return value;
}

float SigmoidSmoother(float position,float max_speed)
{
    float present_goal_speed;
    float postion_x;
    float ln_x;
    ln_x = -log((100 - RATIO)/RATIO);
    if(position>0)
    {
        postion_x = position * ln_x * 2.0f/SMOOTHNESS;
        present_goal_speed = max_speed * Cal_e_data(ln_x - postion_x * SMOOTHNESS);
    }
    else if(position<0)
    {
        postion_x = -position * ln_x * 2.0f/SMOOTHNESS;
        present_goal_speed = -max_speed * Cal_e_data(ln_x - postion_x * SMOOTHNESS);
    }
    else
    {
        present_goal_speed = 0;
    }

    return present_goal_speed;
}

#define DAMPING_VALUE  0.95f

float dampingSmoother(float position,float last_position)
{
    float ce,result_data;
    ce = position - last_position;
    result_data = position - DAMPING_VALUE * ce;
    return result_data;
}


