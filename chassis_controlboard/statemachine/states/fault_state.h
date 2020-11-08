#ifndef __FSM_FAULT_STATE__
#define __FSM_FAULT_STATE__

#include "state_base.h"

typedef struct tagFAULT_STATE
{
    STATE_BASE state;
} FAULT_STATE;

extern FAULT_STATE g_chassisFaultState;

void initChassisFaultState(void);

#endif
