/**************************************************************************************
*    author : hopehu
*    e-mail : huxi@goldenridge.ltd
*    date   : 2019.01.25
*    version: 1.0
 ***************************************************************************************/
#ifndef __W25Q80_H
#define __W25Q80_H

#ifdef __cplusplus
 extern "C" {
#endif 

#include "stm32f4xx.h"
#include "spi.h"
#include "gpio.h"

#define CHIP_ID                            0x13EF
#define W25Q80_FLASH_SIZE                  0x400 /* 1024KB */
#define W25Q80_SECTOR_SIZE                 0x04  /* 4KB */
#define W25Q80_PAGE_SIZE                   0x40  /*64KB */

#define W25Q80_BULK_ERASE_MAX_TIME         250000
#define W25Q80_SECTOR_ERASE_MAX_TIME       3000
#define W25Q80_SUBSECTOR_ERASE_MAX_TIME    800
#define W25Q80_TIMEOUT_VALUE 1000
/************************Flash Sector Address****************************************/
#define W25Q80_FLASH_SECTOR_0     ((uint32_t)0x0000) /* Base @ of Sector 0, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_1     ((uint32_t)0x1000) /* Base @ of Sector 1, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_2     ((uint32_t)0x2000) /* Base @ of Sector 2, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_3     ((uint32_t)0x3000) /* Base @ of Sector 3, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_4     ((uint32_t)0x4000) /* Base @ of Sector 4, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_5     ((uint32_t)0x5000) /* Base @ of Sector 5, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_6     ((uint32_t)0x6000) /* Base @ of Sector 6, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_7     ((uint32_t)0x7000) /* Base @ of Sector 7, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_8     ((uint32_t)0x8000) /* Base @ of Sector 8, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_9     ((uint32_t)0x9000) /* Base @ of Sector 9, 4 Kbyte */
#define W25Q80_FLASH_SECTOR_10    ((uint32_t)0xA000) /* Base @ of Sector 10,4 Kbyte */
#define W25Q80_FLASH_SECTOR_11    ((uint32_t)0xB000) /* Base @ of Sector 11,4 Kbyte */
#define W25Q80_FLASH_SECTOR_12    ((uint32_t)0xC000) /* Base @ of Sector 12,4 Kbyte */
#define W25Q80_FLASH_SECTOR_13    ((uint32_t)0xD000) /* Base @ of Sector 13,4 Kbyte */
#define W25Q80_FLASH_SECTOR_14    ((uint32_t)0xE000) /* Base @ of Sector 14,4 Kbyte */
#define W25Q80_FLASH_SECTOR_15    ((uint32_t)0xF000) /* Base @ of Sector 15,4 Kbyte */
/************************************************************************************/

/***********************CMD**********************/
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define BLOCK_ERASE_CMD 										 0xD8
#define CHIP_ERASE_CMD                       0xC7


#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25Q80_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q80_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q80_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */


#define W25Qx_Enable() 			HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define W25Qx_Disable() 		HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)

#define W25Qx_OK            ((uint8_t)0x00)
#define W25Qx_ERROR         ((uint8_t)0x01)
#define W25Qx_BUSY          ((uint8_t)0x02)
#define W25Qx_TIMEOUT				((uint8_t)0x03)


uint8_t W25Qx_Init(void);
static void	W25Qx_Reset(void);
static uint8_t W25Qx_GetStatus(void);
uint8_t W25Qx_WriteEnable(void);
void W25Qx_Read_ID(uint8_t *ID);
uint8_t W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t W25Qx_Erase_Sector(uint32_t Address);
uint8_t W25Qx_Erase_Block(uint32_t Address);
uint8_t W25Qx_Erase_Chip(void);

  
#ifdef __cplusplus
}
#endif

#endif 

