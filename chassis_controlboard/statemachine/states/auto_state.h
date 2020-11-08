#ifndef __FSM_AUTO_STATE__
#define __FSM_AUTO_STATE__

#include "state_base.h"

typedef struct tagAUTO_STATE
{
    STATE_BASE state;
} AUTO_STATE;

extern AUTO_STATE g_chassisAutoState;

void initChassisAutoState(void);

#endif
