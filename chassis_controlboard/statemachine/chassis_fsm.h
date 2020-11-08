#ifndef __CHASSIS_FSM__
#define __CHASSIS_FSM__

#include "state_base.h"
#include "cmsis_os.h"


/*********************** PUBLIC **********************/
enum CHASSIS_STATE
{
    CHASSIS_STATE_INIT      = -1,
    CHASSIS_STATE_LOCK      = 0,
    CHASSIS_STATE_READY     = 1,
    CHASSIS_STATE_MANNUAL   = 2,
    CHASSIS_STATE_AUTO      = 3,
    CHASSIS_STATE_FAULT     = 4,
    CHASSIS_STATE_BUTTOM
};

enum FSM_EVENT
{
    FSM_EVT_COMMAND_LOCK = 0,
    FSM_EVT_COMMAND_UNLOCK = 1,
    FSM_EVT_COMMAND_STOP = 2,
    FSM_EVT_COMMAND_MANUAL =3 ,
    FSM_EVT_COMMAND_AUTO =4 ,
    FSM_EVT_COMMAND_FAULT =5 ,
    FSM_EVT_COMMAND_FAULT_REC =6 ,
    FSM_EVT_SYSTEM_FAULT = 7 ,
    FSM_EVT_JOYSTICK = 8,
    FSM_EVT_BUTTOM
};

//host computer commod 上位机下发命令集
#define COMMOND_LOCK          0
#define COMMOND_UNLOCK        1
#define COMMOND_STOP          2
#define COMMOND_MANUAL        3
#define COMMOND_AUTO          4
#define COMMOND_FAULT         5
#define COMMOND_FAULT_REC     6
#define COMMOND_FAULT_REPLY   7


void initChassisFsm(void);
void initChassisFsmEvt(void);
void sendEvtToChassisFsm(int evt);
void scheduleChassisFsmEvt(void);
int getChassisFsmCurState(void);

/*********************** PRIVATE **********************/
typedef void (*pfChangState)(struct tagCHASSIS_FSM * fsm, int newState);
typedef void (*pfScheduleEvt)(struct tagCHASSIS_FSM * fsm);
typedef int  (*pfGetCurState)(struct tagCHASSIS_FSM * fsm);
//typedef void (*pfPushEvt)(struct tagCHASSIS_FSM * fsm, int evt);
//typedef void (*pfPushPub)(struct tagCHASSIS_FSM * fsm, int flag);
typedef osStatus (*pfPushEvt)(uint16_t msg);
typedef osStatus (*pfPullEvt)(uint16_t *msg);

typedef struct tagCHASSIS_FSM
{
    pfChangState changeState;
    pfScheduleEvt scheduleEvt;
    pfGetCurState getCurState;
    pfPushEvt pushEvt;
    pfPullEvt pullEvt;
//	pfPushPub pushPub;

    int m_eState;
    struct tagSTATE_BASE * pState;
//  osMessageQId m_evtQueue;
//  osMessageQId m_pubQueue;
    int m_handleEvtFlag;
} CHASSIS_FSM;

extern CHASSIS_FSM g_chassisFsm;
extern const char logFsmCmd[8][15];
extern const char logFsmState[6][12];
extern const char logFsmEvent[10][15];


void clearFsmEvtQueue(void);

#endif


