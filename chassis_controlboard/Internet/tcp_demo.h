#ifndef __TCP_DEMO_H
#define __TCP_DEMO_H
#include "types.h"
#include "stm32f4xx_hal.h"
extern uint16 W5500_tcp_server_port;
extern uint8_t tcp_connectflag ;
void do_tcp_server(void);
#endif

