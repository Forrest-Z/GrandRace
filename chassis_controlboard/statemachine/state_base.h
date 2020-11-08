#ifndef __FSM_STATE_BASE__
#define __FSM_STATE_BASE__

struct tagCHASSIS_FSM;
typedef void (*pfEnter)(struct tagCHASSIS_FSM * fsm);
typedef void (*pfHandleEvt)(struct tagCHASSIS_FSM * fsm, int evt);

typedef struct tagSTATE_BASE
{
    pfEnter enter;
    pfHandleEvt handleEvt;
    //pfFilter filter;
} STATE_BASE;


#endif
