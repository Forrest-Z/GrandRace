#ifndef _SYS_QUEUE_H_
#define _SYS_QUEUE_H_

#include "cmsis_os.h"

#define FSM_PUB_DISABLE									0						// 上报不使能
#define FSM_PUB_ENABLE									1						// 上报使能

enum CHASSIS_EVENT
{
    CHASSIS_PARKING_START_EVT = 0,
    CHASSIS_PARKING_END_EVT,
    CHASSIS_BREAK_PRESS_EVT,
    CHASSIS_BREAK_RECOVERY_EVT,
    CHASSIS_CHARGING_START_EVT,
    CHASSIS_CHARGING_END_EVT,
    CHASSIS_BUTTOM_EVT
};

extern osMessageQId fsmEvtQueueHandle;
extern osMessageQId fsmPubQueueHandle;
extern osMessageQId chassisEvtQueueHandle;


osStatus pushFsmEvt(uint16_t msg);
osStatus pushFsmPubEvt(uint16_t msg);
osStatus pushChassisEvt(uint16_t msg);

osStatus pullFsmEvt(uint16_t *pmsg);
osStatus pullFsmPubEvt(uint16_t *pmsg);
osStatus pullChassisEvt(uint16_t *pmsg);


#endif

