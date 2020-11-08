#pragma once
#ifndef __CONFIG_COM_H__
#define __CONFIG_COM_H__

#include "stdint.h"
void Cal_CRC(unsigned char *crc, unsigned char *src, int size);
//У���
unsigned char checksum(unsigned char *p, unsigned int len);
uint16_t CheckSum16(uint8_t *buf, uint16_t len);

typedef union
{
	unsigned char  B[2];//CRC����
	unsigned short u16_v;
    short s16_v;
}u16;
typedef union
{
	unsigned char  B[2];//CRC����
	int int32_v;
	unsigned int uInt32_v;
}u32;
/*
*�������ֽ�ת��λ16λ
*/
#define BUILD_UINT16_T(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))	
#define BUILD_INT16_T(loByte, hiByte)  ((int16_t) (((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
/*
 * �궨��ʵ�ַ��ؾ���ֵ��x�ﲻ�����Լ��Լ�����䣬�����������
 */
#define MY_ABS(x) (((x) > 0) ? (x) : (-(x)))
 /**
  *  @brief ������λ��0����1 0��ʼ
  */
#define BIT_CLEAN(var,n)        (var) &= ~(1<<(n))   //����var ��nλ������n+1λ����0
#define BIT_SET(var,n)          (var) |=  (1<<(n))   //����var ��nλ������n+1λ����1
#define BIT_GET(var,n)          (((var)>>(n))&0x01)  //��ȡ����var ��nλ������n+1λ��

#endif
