#include "spi_driver.h"

HAL_StatusTypeDef readSpiMultiBytes(uint8_t reg_addr, uint8_t *pbuf, uint8_t len)
{
    uint8_t addr = (0x80 | reg_addr);
    HAL_StatusTypeDef errorcode = HAL_OK;

    IMU_CS_LOW();

    HAL_SPI_Transmit(&hspi2, &addr, 1, 0xFFFFFF);

    errorcode = HAL_SPI_Receive(&hspi2, pbuf, len, 0xFFFFFF);

    IMU_CS_HIGH();

    return errorcode;
}

HAL_StatusTypeDef writeSpiMultiBytes(uint8_t reg_addr, uint8_t *pbuf, uint8_t len)
{
    HAL_StatusTypeDef errorcode = HAL_OK;
    IMU_CS_LOW();

    HAL_SPI_Transmit(&hspi2, &reg_addr, 1, 0xFFFFFF);
    errorcode = HAL_SPI_Transmit(&hspi2, pbuf, len, 0xFFFFFF);

    IMU_CS_HIGH();

    return errorcode;
}

HAL_StatusTypeDef readSpiSingleByte(uint8_t reg_addr, uint8_t *pbuf)
{
    return readSpiMultiBytes(reg_addr, pbuf, 1);
}

HAL_StatusTypeDef writeSpiSingleByte(uint8_t reg_addr, uint8_t dat)
{
    return writeSpiMultiBytes(reg_addr, &dat, 1);
}

