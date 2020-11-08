
#ifndef _CTL_FILTER_H
#define _CTL_FILTER_H
#pragma pack(4)
/*
*一个滤波器，用于过滤掉可能发生的不连续的噪声
*	滤波依据：信号是一个连续的信号，不会发生过大的突变
*/
#define ExtremeFilterMAX 0.5f	//数据可能出现的最大的值
#define ExtremeFilterK		0.1f //不连续阈值
#define ExtremeFilter 	3 //buf长度

typedef struct
{
    float dataBuf[4];
    float inData;
    float outData;
    float max;			//数据可能出现的最大的值
    float k;				//不连续阈值
} ExtremeFilterType;

float extremeFilter(ExtremeFilterType *in);
/*end
*/


#endif
