#ifndef __FSM_LOCK_STATE__
#define __FSM_LOCK_STATE__

#include "state_base.h"

typedef struct tagLOCK_STATE
{
    STATE_BASE state;
} LOCK_STATE;

extern LOCK_STATE g_chassisLockState;

void initChassisLockState(void);

#endif
