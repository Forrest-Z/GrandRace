#include "fault_state.h"
#include "../chassis_fsm.h"
#include "sys_monitor.h"
#include "info_core.h"

FAULT_STATE g_chassisFaultState;

void chassisFaultStateEnter(struct tagCHASSIS_FSM * fsm)
{

}

void chassisFaultStateHandleEvt(struct tagCHASSIS_FSM * fsm, int evt)
{
    switch (evt)
    {
			case FSM_EVT_COMMAND_FAULT_REC:
				if((getSystemWorkState() != SYSTEM_WORK_NORMAL) 
					|| (getTX2CommunicationState() == 0))
				{
					return;
				}
				
        fsm->changeState(fsm, CHASSIS_STATE_LOCK);
			break;
    }
}


void initChassisFaultState()
{
    g_chassisFaultState.state.enter      = chassisFaultStateEnter;
    g_chassisFaultState.state.handleEvt  = chassisFaultStateHandleEvt;
}
