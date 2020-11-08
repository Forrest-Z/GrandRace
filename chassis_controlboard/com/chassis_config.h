/**
******************************************************************************
* @file    chassis_config.h
* @brief   chassis configuration file.
******************************************************************************
* @attention
* COPYRIGHT(c) 2019 Goldenridge Ltd. All rights reserved.
* 本配置文件主要应用于底盘系统的相关参数、软件版本等配置
******************************************************************************
*/

#ifndef _CHASSIS_CONIFIG_H_
#define _CHASSIS_CONIFIG_H_

#include <types.h>
//主板版本.子版本/硬件版本
#define MAIN_VERSION  		"main:V2.3.6/hardware:V2.0" /* 当前软件主版本号 */
/*--------------------------------------------------------------------------*/
#define	__VERSION_USER  	 				//版本发布宏 当需要版本发布时 需要打开
/*--------------------------------------------------------------------------*/

// USE_HAL_DRIVER,STM32F407xx,__LOG,__DBG,__WDG 记得在C/C++选项里添加打开看门狗 ,__WDG
//MicroLib并不支援assert()，所以才会出现错误讯息”，
//在target中钩掉USE MicroLIB编译就可以通过了。
//typeof C++ 勾选 GNU

/*================================================================*/
#ifdef __VERSION_USER
#define	__ONLINE_UPDATE_USER  	  //是否需要在线更新 需要在线更新打开，不需要在线更新屏蔽  Online updating
#define	__COMMUN_USER_NET  		 	  // 网口  USB 屏蔽使用USB 版本发布需要打开网口
#define	__TP_USER  		 	   				// TP 使能 使用打开 不使用屏蔽 typeof C++ 勾选 GNU
#define	__ALARM_USER  		 	   	  // 新的ALARM使能 使用打开 不使用则屏蔽 新的告警使能
//#define __BMD_CHARG_EN          //是否加载充满标志 使用打开 不使用则屏蔽
//#define  __SAFETY_USER				  //主动安全功能 不用

#else
//#define	__ONLINE_UPDATE_USER  	  //是否需要在线更新 需要在线更新打开，不需要在线更新屏蔽  Online updating
#define	__COMMUN_USER_NET
#define	__TP_USER 
#define	__ALARM_USER
#define __UART1_EN_DEBUG
#define __TEST_CASE_EN
#endif


#endif
/************************ (C) COPYRIGHT Goldenridge Ltd *****END OF FILE****/

