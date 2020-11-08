#ifndef __FSM_READY_STATE__
#define __FSM_READY_STATE__

#include "state_base.h"

typedef struct tagREADY_STATE
{
    STATE_BASE state;
} READY_STATE;

extern READY_STATE g_chassisReadyState;

void initChassisReadyState(void);

#endif
