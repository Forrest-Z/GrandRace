#ifndef __FSM_MANNUAL_STATE__
#define __FSM_MANNUAL_STATE__

#include "state_base.h"

typedef struct tagMANNUAL_STATE
{
    STATE_BASE state;
} MANNUAL_STATE;

extern MANNUAL_STATE g_chassisMannualState;

void initChassisMannualState(void);

#endif
