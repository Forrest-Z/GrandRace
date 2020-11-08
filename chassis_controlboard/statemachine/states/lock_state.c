
#include "lock_state.h"
#include "../chassis_fsm.h"

LOCK_STATE g_chassisLockState;

void chassisLockStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

void chassisLockStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
    case FSM_EVT_COMMAND_UNLOCK:
        fsm->changeState(fsm, CHASSIS_STATE_READY);
        break;
    
    //case CHASSIS_STATE_FAULT:
		 case FSM_EVT_COMMAND_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
		 case FSM_EVT_SYSTEM_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
    }
}

void initChassisLockState()
{
    g_chassisLockState.state.enter      = chassisLockStateEnter;
    g_chassisLockState.state.handleEvt  = chassisLockStateHandleEvt;
}

