#ifndef _TX2_POWER_H
#define _TX2_POWER_H

#include <stdint.h>
#include "chassis_com.h"

typedef enum {
    POWEROFF_STATE_INIT  = 0,
    POWEROFF_STATE_BUTTON,
    POWEROFF_STATE_REQUEST,
    POWEROFF_STATE_CLOSE,
} Tx2PoweroffState;


//void TX2Power_Init(void);
void openTx2Power(void);
void TX2Power_Off(void);

void SetPoweroffState(uint8_t state);
uint8_t GetPoweroffState(void);

void handlerTx2PowerOff(void);

#endif

