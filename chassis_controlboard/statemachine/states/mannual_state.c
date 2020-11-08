#include "mannual_state.h"
#include "../chassis_fsm.h"

MANNUAL_STATE g_chassisMannualState;

void chassisMannualStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

//int filterAccMannualState()
//{
//}

void chassisMannualStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
    case FSM_EVT_COMMAND_STOP:  // 
        fsm->changeState(fsm, CHASSIS_STATE_READY);
        break;
    
    case FSM_EVT_COMMAND_LOCK:  
        fsm->changeState(fsm, CHASSIS_STATE_LOCK);
        break;
    
    case FSM_EVT_COMMAND_AUTO:  
        fsm->changeState(fsm, CHASSIS_STATE_AUTO);
        break;
    
    case FSM_EVT_COMMAND_FAULT:  
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
		case FSM_EVT_SYSTEM_FAULT:
        fsm->changeState(fsm, CHASSIS_STATE_FAULT);
        break;
    }
}


void initChassisMannualState()
{
    g_chassisMannualState.state.enter      = chassisMannualStateEnter;
    g_chassisMannualState.state.handleEvt  = chassisMannualStateHandleEvt;
}
