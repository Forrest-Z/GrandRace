#ifndef __SUBSCRIBE_POWERBOARD_H
#define __SUBSCRIBE_POWERBOARD_H

#include "Bms.h"

void bmsProtocolCallBack(void);
void updateBmsProtocolData(Bms_Buffer *p);
void PowerRequestCallBack(void);
void Power_VersionCallback(void);
#endif
