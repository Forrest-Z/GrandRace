
#include "chassis_fsm.h"
#include "states/init_state.h"
#include "states/lock_state.h"
#include "states/ready_state.h"
#include "states/auto_state.h"
#include "states/mannual_state.h"
#include "states/fault_state.h"
#include "sys_monitor.h"
#include "sys_queue.h"
#include "SD_Log_App.h"

CHASSIS_FSM g_chassisFsm =
{
    .m_eState = CHASSIS_STATE_FAULT,	// 初始化为故障态
};

const char logFsmCmd[8][15] = {
  "LOCK_0",
  "UNLOCK_1",
  "STOP_2",
  "MANUAL_3",
  "AUTO_4",
  "FAULT_5",
  "FAULT_REC_6",
  "FAULT_REPLY_7",
};
const char logFsmState[6][12] = {
  "LOCK_0",
  "READY_1",
  "MANNUAL_2",
  "AUTO_3",
  "FAULT_4",
  "BUTTOM",
  
};

const char logFsmEvent[10][15] = {
  "LOCK_0",
  "UNLOCK_1",
  "STOP_2",
  "MANUAL_3",
  "AUTO_4",
  "FAULT_5",
  "FAULT_REC_6",
  "SYSTEM_FAULT_7",
  "JOYSTICK_8",
  "BUTTOM",
};

void changeState(struct tagCHASSIS_FSM * fsm, int newState)
{
    if(newState != -1) //CHASSIS_STATE_INIT
    {
      GR_LOG_INFO("change state from %d %s --> %d %s"
        ,fsm->m_eState,logFsmState[fsm->m_eState], newState,logFsmState[newState]);
    }
    else
    { 
      //CHASSIS_STATE_INIT
      GR_LOG_INFO("change state from %d --> %d", fsm->m_eState, newState);
    }
    if (newState >= CHASSIS_STATE_BUTTOM)
    {
        GR_LOG_ERROR("wrong state enum!");
        return;
    }

    switch (newState)
    {
    case CHASSIS_STATE_INIT:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisInitState;
        break;

    case CHASSIS_STATE_LOCK:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisLockState;
        break;

    case CHASSIS_STATE_READY:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisReadyState;
        break;

    case CHASSIS_STATE_MANNUAL:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisMannualState;
        break;

    case CHASSIS_STATE_AUTO:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisAutoState;
        break;

    case CHASSIS_STATE_FAULT:
        fsm->pState = (struct tagSTATE_BASE *)&g_chassisFaultState;
        break;

    default:
        break;
    }

    //clearFsmEvtQueue();
    fsm->m_eState = newState;
    fsm->pState->enter(fsm);
}

void scheduleEvt(struct tagCHASSIS_FSM * fsm)
{
    uint16_t evtId = FSM_EVT_BUTTOM;

    if (1 == fsm->m_handleEvtFlag)
    {
        GR_LOG_DEBUG("now fsm handling evt!");
        return;
    }

    if(osEventMessage != g_chassisFsm.pullEvt(&evtId))
    {
        return;
    }

    GR_LOG_INFO("fsm handling new evt, id = %d %s.", evtId,logFsmEvent[evtId]);
    fsm->m_handleEvtFlag = 1;
    fsm->pState->handleEvt(fsm, evtId);
	  pushFsmPubEvt(fsm->m_eState);
    fsm->m_handleEvtFlag = 0;
}

int getCurState(struct tagCHASSIS_FSM * fsm)
{
    return fsm->m_eState;
}

void initChassisFsm()
{
    GR_LOG_INFO("init chassis fsm ..."); 
    g_chassisFsm.changeState    = changeState;
    g_chassisFsm.scheduleEvt    = scheduleEvt;
    g_chassisFsm.getCurState    = getCurState;
    g_chassisFsm.pushEvt        = pushFsmEvt;	//pushEvt;
    g_chassisFsm.pullEvt        = pullFsmEvt;

    g_chassisFsm.m_handleEvtFlag  = 0;

    // init all states
    initChassisInitState();
    initChassisLockState();
    initChassisReadyState();
    initChassisAutoState();
    initChassisMannualState();
    initChassisFaultState();

    g_chassisFsm.changeState(&g_chassisFsm, CHASSIS_STATE_FAULT);	
}

void sendEvtToChassisFsm(int evt)
{
    GR_LOG_INFO("fsm receive evt,id=%d.%s", evt,logFsmEvent[evt]);
    g_chassisFsm.pushEvt(evt);
}

void initChassisFsmEvt(void)
{
    GR_LOG_INFO("init chassis fsm evt.");
	  sendEvtToChassisFsm(FSM_EVT_SYSTEM_FAULT);
}

void scheduleChassisFsmEvt(void)
{
    g_chassisFsm.scheduleEvt(&g_chassisFsm);
}

int getChassisFsmCurState(void)
{
    return g_chassisFsm.getCurState(&g_chassisFsm);
}

void clearFsmEvtQueue(void)
{
//    while (osOK != osMessageGet(g_chassisFsm.m_evtQueue, 1).status)
//    {
//        continue;
//    }
}







