
#include "init_state.h"
#include "../chassis_fsm.h"

INIT_STATE g_chassisInitState;

void chassisInitStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

void chassisInitStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
			case FSM_EVT_COMMAND_STOP:
				fsm->changeState(fsm, CHASSIS_STATE_READY);
				break;
			case FSM_EVT_COMMAND_FAULT:
			case FSM_EVT_SYSTEM_FAULT:
				fsm->changeState(fsm, CHASSIS_STATE_FAULT);
				break;
    }
}

void initChassisInitState()
{
    g_chassisInitState.state.enter      = chassisInitStateEnter;
    g_chassisInitState.state.handleEvt  = chassisInitStateHandleEvt;
}

