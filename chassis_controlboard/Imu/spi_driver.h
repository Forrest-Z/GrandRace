#ifndef _SPI_DRIVER_H_
#define _SPI_DRIVER_H_

#include "spi.h"
#pragma pack(1)

#define IMU_CS_HIGH()    		HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin,GPIO_PIN_SET)
#define IMU_CS_LOW()      	HAL_GPIO_WritePin(IMU_CS_GPIO_Port, IMU_CS_Pin,GPIO_PIN_RESET)

#define IMU_OK 							HAL_OK
#define IMU_ERROR 					HAL_ERROR

HAL_StatusTypeDef readSpiMultiBytes (uint8_t reg_addr, uint8_t *pbuf, uint8_t len);
HAL_StatusTypeDef writeSpiMultiBytes(uint8_t reg_addr, uint8_t *pbuf, uint8_t len);
HAL_StatusTypeDef readSpiSingleByte (uint8_t reg_addr, uint8_t *pbuf);
HAL_StatusTypeDef writeSpiSingleByte(uint8_t reg_addr, uint8_t dat);

#endif


