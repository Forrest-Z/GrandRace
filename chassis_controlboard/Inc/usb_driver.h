#ifndef _USB_DRIVER_H
#define _USB_DRIVER_H

#include <stdint.h>

void Driver_USB_Init(void);
void Driver_USB_Buffer_Init(void);
void Driver_USB_Receive(uint8_t *pbuf, uint32_t *len);
uint32_t Driver_USB_StateCheck(void);
//uint8_t Driver_USB_Read(void);
void Driver_USB_Write(uint8_t *pbuf, uint32_t len);
void Driver_USB_Itf_SofISR(void);
int32_t Driver_USB_Itf_Write( uint8_t *p_buf, uint32_t length );
uint32_t Driver_USB_Itf_Available( void );
uint8_t Driver_USB_Itf_Getch( void );
#endif


