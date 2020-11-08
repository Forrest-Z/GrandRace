#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "w5500.h"
#include "socket.h"

#define UART_BUFSIZE  2048  //512;

#pragma pack(1)

typedef struct
{
    int RX_head;
    int RX_tail;
    int TX_head;
    int TX_tail;
    unsigned char RX[UART_BUFSIZE];
//    unsigned char TX[UART_BUFSIZE/2];
} BUFFER_DATA;

extern BUFFER_DATA ultrasonicBoard_buffer;
extern BUFFER_DATA powerBoard_buffer;
extern BUFFER_DATA sensorBoard_buffer;
extern BUFFER_DATA TX2Board_buffer;
extern BUFFER_DATA LightstripBoard_buffer;

unsigned long int readSysTime_ms(void);
void updateReceiveDataCount(void);
void GetReceiveData(void);
void receiveDataToRXbuffer(unsigned char data,BUFFER_DATA *buffer_data);
void publishData(BUFFER_DATA *buffer_data);

void transmitTX2Data(int length,unsigned char *data);
void transmitPowerBoardData(int length,unsigned char *data);
void transmitJoyBoardData(int length,unsigned char *data);
void transmitUltrasonicBoardData(int length,unsigned char *data);
void transmitLightstripBoardData(int length,unsigned char *data);

void info_delay_ms(unsigned long int time_data);
void set_ult_reboot( uint8_t flag);

#endif
