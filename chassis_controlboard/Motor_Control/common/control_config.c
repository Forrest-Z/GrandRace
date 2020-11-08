#include "control_config.h"

#if CHASSIS_TYPE == WHEEL_CHAIR1
float LEFT_PID_KP = 0.2f;
float LEFT_PID_KI = 0.01f;
float LEFT_PID_KD = 0.01f;

float RIGHT_PID_KP = 0.2f;
float RIGHT_PID_KI = 0.01f;
float RIGHT_PID_KD = 0.01f;

float ANGULAR_PID_KP = 0.5f;
float ANGULAR_PID_KI = 0.2f;
float ANGULAR_PID_KD = 0.0f;
#endif
#if CHASSIS_TYPE == WHEEL_CHAIR2
float LEFT_PID_KP = 0.15f;
float LEFT_PID_KI = 0.2f;
float LEFT_PID_KD = 0.0f;

float RIGHT_PID_KP = 0.15f;
float RIGHT_PID_KI = 0.2f;
float RIGHT_PID_KD = 0.0f;

float ANGULAR_PID_KP = 0.5f;
float ANGULAR_PID_KI = 0.2f;
float ANGULAR_PID_KD = 0.0f;
#endif
#if CHASSIS_TYPE == ROAM
float LEFT_PID_KP = 0.15f;
float LEFT_PID_KI = 0.2f;
float LEFT_PID_KD = 0.0f;

float RIGHT_PID_KP = 0.15f;
float RIGHT_PID_KI = 0.2f;
float RIGHT_PID_KD = 0.0f;

float ANGULAR_PID_KP = 0.5f;
float ANGULAR_PID_KI = 0.2f;
float ANGULAR_PID_KD = 0.0f;
#endif