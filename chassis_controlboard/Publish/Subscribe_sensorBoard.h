#ifndef __SUBSCRIBE_SENSORBOARD_H
#define __SUBSCRIBE_SENSORBOARD_H

#include "joystick.h"

void joystickProtocolCallBack(void);
uint8_t getJoystickRawData(Joystick_TypeDef *pmsg);
void Joy_VersionCallback(void);

void led_VersionCallback(void);;
#endif
