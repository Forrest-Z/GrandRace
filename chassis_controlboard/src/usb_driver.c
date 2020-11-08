#include "usb_driver.h"
#include "usbd_cdc.h"
#include <string.h>
#include "usb_device.h"
#include "buffer.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define USB_RX_DATA_SIZE  2048
#define USB_TX_DATA_SIZE  2048
#define USB_RX_BUF_SIZE   2048//(1024*4)


uint8_t UserRxBuffer[USB_RX_DATA_SIZE];/* Received Data over USB are stored in this buffer */
uint8_t UserTxBuffer[USB_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */
uint8_t UserTxBufferForUSB[USB_TX_DATA_SIZE];/* Received Data over UART (CDC interface) are stored in this buffer */

uint32_t BuffLength;
static uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
static uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */

//static BOOL is_opened = FALSE;
//static BOOL is_reopen = FALSE;
volatile uint8_t usb_rx_full = 0;


static uint8_t  rxd_buffer[USB_RX_BUF_SIZE];
//static uint32_t rxd_length    = 0;
uint32_t rxd_BufPtrIn  = 0;
static uint32_t rxd_BufPtrOut = 0;

uint32_t usb_cdc_debug_cnt[16] = {0};


void   Driver_USB_Itf_TxISR(void);
static uint32_t Driver_USB_Itf_TxAvailable(void);


extern USBD_HandleTypeDef hUsbDeviceFS;

void Driver_USB_Itf_Init(void)
{
//	is_opened = FALSE;
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBuffer, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBuffer);

    BuffLength            = 0;
    UserTxBufPtrIn        = 0;
    UserTxBufPtrOut       = 0;

    rxd_BufPtrIn          = 0;
    rxd_BufPtrOut         = 0;
}


uint8_t Driver_USB_Itf_IsAvailable( void )
{
    if( rxd_BufPtrIn != rxd_BufPtrOut ) return TRUE;

    return FALSE;
}



uint32_t Driver_USB_Itf_Available( void )
{
    uint32_t length;

    //__disable_irq();
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    length = (USB_RX_BUF_SIZE + rxd_BufPtrIn - rxd_BufPtrOut) % USB_RX_BUF_SIZE;
    //__enable_irq();
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    return length;
}


void Driver_USB_Itf_SofISR(void)
{
    uint32_t rx_buf_length;


    rx_buf_length = USB_RX_DATA_SIZE - Driver_USB_Itf_Available() - 1;

    if (usb_rx_full == 1)
    {
        if (rx_buf_length > CDC_DATA_FS_MAX_PACKET_SIZE)
        {
            USBD_CDC_ReceivePacket(&hUsbDeviceFS);
            usb_rx_full = 0;
        }
    }
    Driver_USB_Itf_TxISR();
}

void Driver_USB_Itf_TxISR(void)
{
    uint32_t buffptr;
    uint32_t buffsize;

    USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;

    if(hcdc == NULL)
    {
        return;
    }
    if(hcdc->TxState != 0)
    {
        return;
    }

    if(UserTxBufPtrOut != UserTxBufPtrIn)
    {
        if(UserTxBufPtrOut > UserTxBufPtrIn) /* Rollback */
        {
            buffsize = USB_TX_DATA_SIZE - UserTxBufPtrOut;
        }
        else
        {
            buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
        }

        if (buffsize%CDC_DATA_FS_MAX_PACKET_SIZE == 0 && buffsize > 0)
        {
            buffsize -= 1;
        }

        buffptr = UserTxBufPtrOut;

        memcpy(UserTxBufferForUSB, (uint8_t*)&UserTxBuffer[buffptr], buffsize);
        USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferForUSB, buffsize);

        if(USBD_CDC_TransmitPacket(&hUsbDeviceFS) == USBD_OK)
        {
            UserTxBufPtrOut += buffsize;
            if (UserTxBufPtrOut == USB_TX_DATA_SIZE)
            {
                UserTxBufPtrOut = 0;
            }
        }
    }
}

void Driver_USB_Itf_Receive(uint8_t* Buf, uint32_t *Len)
{
    uint32_t i;
    uint32_t rx_buf_length;


    for( i=0; i<*Len; i++ )
    {
//		if( i == 1)
//		{
//			debugPrint(86);
//		}
        rxd_buffer[rxd_BufPtrIn] = Buf[i];
        //TX2Board_buffer.RX[TX2Board_buffer.RX_head] = Buf[i];
        rxd_BufPtrIn++;

        /* To avoid buffer overflow */
        if(rxd_BufPtrIn == USB_RX_BUF_SIZE)
        {
            rxd_BufPtrIn = 0;
        }

        //TX2Board_buffer.RX_head = rxd_BufPtrIn;
    }

    rx_buf_length = USB_RX_DATA_SIZE - Driver_USB_Itf_Available() - 1;

    if (rx_buf_length > CDC_DATA_FS_MAX_PACKET_SIZE)
    {
        USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    }
    else
    {
        usb_rx_full = 1;
    }
}

int32_t Driver_USB_Itf_Write( uint8_t *p_buf, uint32_t length )
{
    uint32_t i;
    uint32_t ptr_index;


    if( hUsbDeviceFS.pClassData == NULL )
    {
        return -1;
    }
//  if( is_opened == FALSE && is_reopen == FALSE )
//  {
//    return -1;
//  }
    if( hUsbDeviceFS.dev_state != USBD_STATE_CONFIGURED )
    {
        return -1;
    }
    if (length >= Driver_USB_Itf_TxAvailable())
    {
        return 0;
    }

    //__disable_irq();
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    ptr_index = UserTxBufPtrIn;


    for (i=0; i<length; i++)
    {
//		if(i == 1)
//		{
//			debugPrint(87);
//		}
        UserTxBuffer[ptr_index] = p_buf[i];

        ptr_index++;

        /* To avoid buffer overflow */
        if(ptr_index == USB_TX_DATA_SIZE)
        {
            ptr_index = 0;
        }
    }
    UserTxBufPtrIn = ptr_index;
    //__enable_irq();
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    return length;
}


uint32_t Driver_USB_Itf_TxAvailable( void )
{
    uint32_t length = 0;

    //__disable_irq();
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    length = (USB_TX_DATA_SIZE + UserTxBufPtrIn - UserTxBufPtrOut) % USB_TX_DATA_SIZE;
    length = USB_TX_DATA_SIZE - length;
    //__enable_irq();
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    return length;
}


int32_t Driver_USB_Itf_Peek( void )
{

    if( rxd_BufPtrIn == rxd_BufPtrOut ) return -1;


    return rxd_buffer[rxd_BufPtrOut];
}


uint8_t Driver_USB_Itf_IsConnected( void )
{
    if( hUsbDeviceFS.dev_config == 0
//    || is_opened == FALSE
            || hUsbDeviceFS.pClassData == NULL )
    {
        return FALSE;
    }

    return TRUE;
}


uint8_t Driver_USB_Itf_Getch( void )
{
    uint8_t ch = 0;
    uint32_t buffptr;


    while(1)
    {
        if(Driver_USB_Itf_IsAvailable())
            break;
    }

    buffptr = rxd_BufPtrOut;

    ch = rxd_buffer[buffptr];

    //__disable_irq();
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    rxd_BufPtrOut += 1;
    if (rxd_BufPtrOut == USB_RX_BUF_SIZE)
    {
        rxd_BufPtrOut = 0;
    }
    //__enable_irq();
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    return ch;
}

void Driver_USB_Init(void)
{
    MX_USB_DEVICE_Init();
}

void Driver_USB_Buffer_Init(void)
{
    Driver_USB_Itf_Init();
}

void Driver_USB_Receive(uint8_t *pbuf, uint32_t *len)
{
    Driver_USB_Itf_Receive(pbuf, len);

//	uint32_t i;
//
//	for(i = 0; i < *len; i++)
//	{
//		receiveDataToRXbuffer(pbuf[i], &TX2Board_buffer);
//	}
}

uint32_t Driver_USB_StateCheck(void)
{
    return Driver_USB_Itf_Available();
}

//uint8_t Driver_USB_Read(void)
//{
//	return Driver_USB_Itf_Getch();
//}

void Driver_USB_Write(uint8_t *pbuf, uint32_t len)
{
    Driver_USB_Itf_Write(pbuf, len);
    Driver_USB_Itf_SofISR();
}


