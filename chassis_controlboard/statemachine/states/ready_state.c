#include "ready_state.h"
#include "chassis_fsm.h"

READY_STATE g_chassisReadyState;

void chassisReadyStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

void chassisReadyStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
    case FSM_EVT_COMMAND_MANUAL:
        fsm->changeState(fsm, CHASSIS_STATE_MANNUAL);
        break;
    
    case FSM_EVT_JOYSTICK:
        fsm->changeState(fsm, CHASSIS_STATE_MANNUAL);
        break;
    
    case FSM_EVT_COMMAND_AUTO:
        fsm->changeState(fsm, CHASSIS_STATE_AUTO);
        break;
    
    case FSM_EVT_COMMAND_LOCK:
        fsm->changeState(fsm, CHASSIS_STATE_LOCK);
        break;
    
    case FSM_EVT_COMMAND_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
		case FSM_EVT_SYSTEM_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
    }
}


void initChassisReadyState()
{
    g_chassisReadyState.state.enter      = chassisReadyStateEnter;
    g_chassisReadyState.state.handleEvt  = chassisReadyStateHandleEvt;
}
