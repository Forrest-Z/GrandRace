#pragma once
#ifndef __CONFIG_COM_H__
#define __CONFIG_COM_H__

#include "stdint.h"
void Cal_CRC(unsigned char *crc, unsigned char *src, int size);
//校验和
unsigned char checksum(unsigned char *p, unsigned int len);
uint16_t CheckSum16(uint8_t *buf, uint16_t len);

typedef union
{
	unsigned char  B[2];//CRC数据
	unsigned short u16_v;
    short s16_v;
}u16;
typedef union
{
	unsigned char  B[2];//CRC数据
	int int32_v;
	unsigned int uInt32_v;
}u32;
/*
*将两个字节转换位16位
*/
#define BUILD_UINT16_T(loByte, hiByte) ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))	
#define BUILD_INT16_T(loByte, hiByte)  ((int16_t) (((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))
/*
 * 宏定义实现返回绝对值（x里不能有自加自减的语句，否则变量出错）
 */
#define MY_ABS(x) (((x) > 0) ? (x) : (-(x)))
 /**
  *  @brief 变量的位清0和置1 0开始
  */
#define BIT_CLEAN(var,n)        (var) &= ~(1<<(n))   //变量var 的n位（即第n+1位）清0
#define BIT_SET(var,n)          (var) |=  (1<<(n))   //变量var 的n位（即第n+1位）置1
#define BIT_GET(var,n)          (((var)>>(n))&0x01)  //读取变量var 的n位（即第n+1位）

#endif
