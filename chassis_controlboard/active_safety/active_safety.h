#ifndef _ACTIVE_SAFETY_H
#define _ACTIVE_SAFETY_H

#include "chassis_config.h"

#ifdef __SAFETY_USER

#define ULTRASONIC_NUM 12

#define SAFE_DISTANCE_FRONT  180 			//单位mm
#define BRAKE_DISTANCE_FRONT  100

#define SAFE_DISTANCE_BACK  180 			//单位mm
#define BRAKE_DISTANCE_BACK  100

#define SAFE_DISTANCE_LEFT  100 			//单位mm
#define BRAKE_DISTANCE_LEFT  50

#define SAFE_DISTANCE_RIGHT  100 			//单位mm
#define BRAKE_DISTANCE_RIGHT  50

void active_safetyHanlder(float *inputGoalControlSpeed,float *outputActiveSafetySpeed);
#endif

#endif //_ACTIVE_SAFETY_H
