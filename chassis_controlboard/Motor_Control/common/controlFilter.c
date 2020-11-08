
#include "controlFilter.h"
#include "math.h"

/*
*一个滤波器，用于过滤掉可能发生的不连续的噪声
*	滤波依据：信号是一个连续的信号，不会发生过大的突变
*/
float extremeFilter(ExtremeFilterType *in)
{
//	float sum = 0;
    //如果数据非法

    //return in->inData;
    if(fabs(in->inData)>3)
    {
        if(fabs(in->outData)>3)
        {
            return in->outData = 0;
        }
        return in->outData;
    }
//
//	return in->outData = in->inData;

    if(fabs(in->inData - in->outData) > in->max)
    {
        return in->outData += in->k * (in->inData - in->outData);
    }

    return in->outData = in->inData;



//
//	//循环缓存
//	for(int i=0;i<ExtremeFilter-1;i++)
//	{
//		in->dataBuf[i] = in->dataBuf[i+1];
//	}
//	in->dataBuf[ExtremeFilter-1] = in->inData;
//
//
//	//滤波操作
//	//判断不连续
//
//
//	/*
//	方法1.
//	去掉最大和最小的量，取均值
//	去掉离均值过远的量，然后求新的均值
//		//比较通用
//	*/
//
//	for(int i=0;i<ExtremeFilter;i++)
//	{
//		sum += in->dataBuf[i];
//	}
//	sum /= ExtremeFilter;
//
//	//去掉极值点
//	for(int i=0;i<ExtremeFilter;i++)
//	{
//		if(fabs(in->dataBuf[i] - sum)>in->k)
//		{
//			in->dataBuf[i] = sum;
//		}
//	}
//
//	//重新求均值
//		for(int i=0;i<ExtremeFilter;i++)
//	{
//		sum += in->dataBuf[i];
//	}
//	sum /= ExtremeFilter;
//
//	in->outData = sum;
//
//	/*
//	方法2.
//	缓存，判读buf中是否存在跳变的值，有就不用
//		会刷新不及时
//	*/
//
//	return in->outData;
}

