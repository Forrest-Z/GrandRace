#include "auto_state.h"
#include "chassis_fsm.h"

AUTO_STATE g_chassisAutoState;

void chassisAutoStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

void chassisAutoStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
    case FSM_EVT_COMMAND_STOP:
        fsm->changeState(fsm, CHASSIS_STATE_READY);
        break;
    
    case FSM_EVT_COMMAND_LOCK:
        fsm->changeState(fsm, CHASSIS_STATE_LOCK);
        break;
    
    case FSM_EVT_JOYSTICK:
        fsm->changeState(fsm, CHASSIS_STATE_MANNUAL);
        break;
    
    case FSM_EVT_COMMAND_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
		
		case FSM_EVT_SYSTEM_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;		
    }
}


void initChassisAutoState()
{
    g_chassisAutoState.state.enter      = chassisAutoStateEnter;
    g_chassisAutoState.state.handleEvt  = chassisAutoStateHandleEvt;
}
