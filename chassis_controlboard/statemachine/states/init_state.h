#ifndef __FSM_INIT_STATE__
#define __FSM_INIT_STATE__

#include "state_base.h"

typedef struct tagINIT_STATE
{
    STATE_BASE state;
} INIT_STATE;

extern INIT_STATE g_chassisInitState;

void initChassisInitState(void);

#endif
