#ifndef __INFO_CORE_H__
#define __INFO_CORE_H__

#include "callback.h"
#include "buffer.h"
#pragma pack(1)
#define SUBSCRIBE_MAX_SIZE 40
#define PUBLISH_MAX_SIZE   40

#define PUBLISH_MAX_SYS_ID   4
#define FRAME_HEAD_SIZE     10
#define FRAME_DATA_SIZE          200
#define FRAME_SIZE               210

//#define TX2_HEARTSINGAL_ID         				1
//#define POWERBOARD_HEARTSINGAL_ID  				2
//#define SENSORBOARD_HEARTSINGAL_ID 				3


//Slave device
#define MASTER_DEVICE 0
#define SLAVE_DEVICE  1

#define CON_ID 0
#define TX2_ID 1
#define POW_ID 2
#define JOYS_ID 3 //
#define ULT_ID 4 //
#define LED_ID 5

#define TX2_HEARTSINGAL_HZ                1
#define POWER_BOARD_HEARTSINGAL_HZ        1
#define SENSOR_BOARD_HEARTSINGAL_HZ       1
#define ULTRASONIC_BOARD_HEARTSINGAL_HZ   1
#define LIGHTSTRIP_BOARD_HEARTSINGAL_HZ   1

#define HEARTSINGAL_DATA1 0xA1
#define HEARTSINGAL_DATA2 0xA2
#define HEARTSINGAL_DATA3 0xA3
#define HEARTSINGAL_DATA4 0xA4

#define HEAD_DATA1  0x12
#define HEAD_DATA2  0x34

#define TAIL_DATA1  0x43
#define TAIL_DATA2  0x21

#define ACK_DATA1  0x11
#define ACK_DATA2  0x22
#define ACK_DATA3  0x33
#define ACK_DATA4  0x44

typedef struct {
    uint8_t TX2_COMLOGFLAG;
    uint8_t POWER_COMLOGFLAG;
    uint8_t JOY_COMLOGFLAG;
    uint8_t ULT_COMLOGFLAG;
} COMLOGFLAG;
typedef struct
{
    unsigned char id_m;
    unsigned char id_s;
    void* data;
    void(*initCallBack)();
    void(*callback)();
} Subscribe_struct;

typedef struct
{
    //HEAD                    //2
    unsigned char crc[2];     //2
    unsigned char frame_size; //1
    unsigned char reserved[2];//2
    unsigned char frame_id_m; //1
    unsigned char frame_id_s; //1
    unsigned char ack;        //1
    unsigned char frame_buffer[FRAME_DATA_SIZE];
    //END                     //2
} FRAME_DATA;

typedef struct
{
    unsigned char send_ack;
    unsigned char resend_num;
    unsigned char resend_set_num;
    unsigned long int send_time;
    unsigned long int curTime;
    unsigned long int over_time;

} publishAckFlag;

typedef struct
{
    FRAME_DATA     sFrameData;
    publishAckFlag sAck;
} PUBLISH_FRAME_STRUCT;

typedef struct
{
    unsigned char id_m;
    unsigned char id_s;
    void* data;
    PUBLISH_FRAME_STRUCT* sFrame;
    void(*initCallBack)();
} Publish_struct;

typedef struct
{
    unsigned long int curTime;
    unsigned long int lastTime;
    unsigned long int offsetTime;
} HEART_SINGAL_TIME_STRUCT;

typedef struct
{
    unsigned char id_m;
    unsigned char id_s;
    HEART_SINGAL_TIME_STRUCT* recordTime;
    void(*publishCallBack)(int,unsigned char*);
} HEART_SINGAL_STRUCT;

typedef struct
{
    Subscribe_struct* pSub[SUBSCRIBE_MAX_SIZE];
    Publish_struct* pPub[PUBLISH_MAX_SIZE];
    HEART_SINGAL_STRUCT* pHeart[6];
    int comState[6];
    unsigned char device_type;     //This device type
    unsigned char device_id;       //This device id
} COM_STRUCK;

extern COM_STRUCK sCom;

void DealAllReceiveData(void);
void initTransferProtocol(unsigned char device_type,unsigned char device_id);
int receiveHanlder(BUFFER_DATA *buffer_data);
void publishFrameData(unsigned char id_m,unsigned char id_s, unsigned char *data, int size);
void infoSpinOnceHanlder(void);
void resetACKStatus(unsigned char id_m,unsigned char id_s);
unsigned char getJOYCommunicationState(void);
unsigned char getBMSCommunicationState(void);
unsigned char getTX2CommunicationState(void);
unsigned char getUltrasonicCommunicationState(void);
unsigned long int getErrorCnt(void);


#define COM_STATE_OK 									0  //正常
#define COM_STATE_ERR 							  1  //异常
typedef union
{
    uint8_t   u8_v;
    struct
    {
        __IO uint8_t b00_tx2_err: 1;    //tx2通信异常
        __IO uint8_t b01_timeout_err: 1;    //通信连接超时
        uint8_t b02: 1;
        uint8_t b03: 1;
        uint8_t b04: 1;
        uint8_t b05: 1;
        uint8_t b06: 1;
        uint8_t b07: 1;
    } flag;
} eCOMM_STATE;    //sizeof(eCOMM_STATE) 为 1

uint8_t getCommState(void);
uint8_t getLedState(void);


#endif

