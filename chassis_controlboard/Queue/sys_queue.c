#include "sys_queue.h"

// 发送状态机事件消息
osStatus pushFsmEvt(uint16_t msg)
{
    return osMessagePut(fsmEvtQueueHandle, msg, osWaitForever);
//	return osOK;
}

// 发送状态机上报事件消息
osStatus pushFsmPubEvt(uint16_t msg)
{
    return osMessagePut(fsmPubQueueHandle, msg, osWaitForever);
//	return osOK;
}

// 发送底盘事件消息
osStatus pushChassisEvt(uint16_t msg)
{
    return osMessagePut(chassisEvtQueueHandle, msg, osWaitForever);
}

// 接收状态机事件消息
osStatus pullFsmEvt(uint16_t *pmsg)
{
    osEvent evt;

    evt = osMessageGet(fsmEvtQueueHandle, 1);

    if(evt.status == osEventMessage)
    {
        *pmsg = evt.value.v;
    }

    return evt.status;

//	return osOK;
}

// 接收状态机上报事件消息
osStatus pullFsmPubEvt(uint16_t *pmsg)
{
    osEvent evt;

    evt = osMessageGet(fsmPubQueueHandle, 1);

    if(evt.status == osEventMessage)
    {
        *pmsg = evt.value.v;
    }

    return evt.status;

//	return osOK;
}

// 接收底盘事件消息
osStatus pullChassisEvt(uint16_t *pmsg)
{
    osEvent evt;

    evt = osMessageGet(chassisEvtQueueHandle, 1);

    if(evt.status == osEventMessage)
    {
        *pmsg = evt.value.v;
    }

    return evt.status;
}



