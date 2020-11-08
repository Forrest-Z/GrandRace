#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_com.h"



void InvertUint8(unsigned char *dBuf, unsigned char *srcBuf)
{
	int i;
	unsigned char tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 8; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (7 - i);
	}
	dBuf[0] = tmp[0];
}


void InvertUint16(unsigned short *dBuf, unsigned short *srcBuf)
{
	int i;
	unsigned short tmp[4];
	tmp[0] = 0;
	for (i = 0; i < 16; i++)
	{
		if (srcBuf[0] & (1 << i))
			tmp[0] |= 1 << (15 - i);
	}
	dBuf[0] = tmp[0];
}

void CRC16_CCITT(unsigned char *crc, unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		InvertUint8(&wChar, &wChar);
		wCRCin ^= (wChar << 8);
		for (int i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else wCRCin = wCRCin << 1;
		}
	}
	InvertUint16(&wCRCin, &wCRCin);
	memcpy(crc, &wCRCin, 2);
}

void Cal_CRC(unsigned char *crc, unsigned char *src, int size)
{
	CRC16_CCITT(crc, src, size);
}

//У���
unsigned char checksum(unsigned char *p, unsigned int len)
{
	unsigned char rt = 0;
	unsigned int i = 0;
	for (i = 0; i < len; i++)
	{
		rt = (p[i] + rt) & 0xff;
	}
	return rt;
}

uint16_t CheckSum16(uint8_t *buf, uint16_t len) //buf为数组，len为数组长�?
{
    uint16_t i, ret = 0;

    for(i=0; i<len; i++)
    {
        ret += *(buf++);
    }
     ret = ~ret;
    return ret;
}

