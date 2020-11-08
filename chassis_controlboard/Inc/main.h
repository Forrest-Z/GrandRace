/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H__
#define __MAIN_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define C_R_FR_Pin GPIO_PIN_2
#define C_R_FR_GPIO_Port GPIOF
#define C_R_EN_Pin GPIO_PIN_3
#define C_R_EN_GPIO_Port GPIOF
#define C_R_BRK_Pin GPIO_PIN_4
#define C_R_BRK_GPIO_Port GPIOF
#define C_R_ALM_Pin GPIO_PIN_5
#define C_R_ALM_GPIO_Port GPIOF
#define C_R_SV_Pin GPIO_PIN_6
#define C_R_SV_GPIO_Port GPIOF
#define C_R_SPEED_Pin GPIO_PIN_7
#define C_R_SPEED_GPIO_Port GPIOF
#define C_L_SV_Pin GPIO_PIN_8
#define C_L_SV_GPIO_Port GPIOF
#define C_L_SPEED_Pin GPIO_PIN_9
#define C_L_SPEED_GPIO_Port GPIOF
#define C_L_FR_Pin GPIO_PIN_0
#define C_L_FR_GPIO_Port GPIOC
#define C_L_EN_Pin GPIO_PIN_1
#define C_L_EN_GPIO_Port GPIOC
#define C_L_BRK_Pin GPIO_PIN_2
#define C_L_BRK_GPIO_Port GPIOC
#define C_L_ALM_Pin GPIO_PIN_3
#define C_L_ALM_GPIO_Port GPIOC
#define P_DRVP_Pin GPIO_PIN_4
#define P_DRVP_GPIO_Port GPIOA
#define W5500_NSS_Pin GPIO_PIN_4
#define W5500_NSS_GPIO_Port GPIOC
#define W5500_INT_Pin GPIO_PIN_5
#define W5500_INT_GPIO_Port GPIOC
#define W5500_RST_Pin GPIO_PIN_0
#define W5500_RST_GPIO_Port GPIOB
#define H_R_U_Pin GPIO_PIN_9
#define H_R_U_GPIO_Port GPIOE
#define H_R_V_Pin GPIO_PIN_11
#define H_R_V_GPIO_Port GPIOE
#define H_R_W_Pin GPIO_PIN_13
#define H_R_W_GPIO_Port GPIOE
#define IMU_INI_Pin GPIO_PIN_11
#define IMU_INI_GPIO_Port GPIOB
#define IMU_CS_Pin GPIO_PIN_12
#define IMU_CS_GPIO_Port GPIOB
#define IMU_SCK_Pin GPIO_PIN_13
#define IMU_SCK_GPIO_Port GPIOB
#define IMU_MISO_Pin GPIO_PIN_14
#define IMU_MISO_GPIO_Port GPIOB
#define IMU_MOSI_Pin GPIO_PIN_15
#define IMU_MOSI_GPIO_Port GPIOB
#define H_L_U_Pin GPIO_PIN_12
#define H_L_U_GPIO_Port GPIOD
#define H_L_V_Pin GPIO_PIN_13
#define H_L_V_GPIO_Port GPIOD
#define H_L_W_Pin GPIO_PIN_14
#define H_L_W_GPIO_Port GPIOD
#define BREAK_Pin GPIO_PIN_2
#define BREAK_GPIO_Port GPIOG
#define TX2_POWER_Pin GPIO_PIN_3
#define TX2_POWER_GPIO_Port GPIOG
#define LED_GREEN_Pin GPIO_PIN_4
#define LED_GREEN_GPIO_Port GPIOG
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOG
#define SD_EXIT_FLAG_Pin GPIO_PIN_15
#define SD_EXIT_FLAG_GPIO_Port GPIOA

/* ########################## Assert Selection ############################## */
/**
  * @brief Uncomment the line below to expanse the "assert_param" macro in the 
  *        HAL drivers code
  */
/* #define USE_FULL_ASSERT    1U */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
