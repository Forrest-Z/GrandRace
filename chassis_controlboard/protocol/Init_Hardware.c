#include "Init_Hardware.h"
#include "buffer.h"
#include "usart.h"
void init_power_board_communication(void)
{
    HAL_UART_Receive_DMA(&huart3, powerBoard_buffer.RX, UART_BUFSIZE);
}
void init_sensor_board_communication(void)
{
    HAL_UART_Receive_DMA(&huart1, sensorBoard_buffer.RX, UART_BUFSIZE);
}

void init_ultrasonic_board_communication(void)
{
    HAL_UART_Receive_DMA(&huart4, ultrasonicBoard_buffer.RX, UART_BUFSIZE);
}

void init_lightstrip_board_communication(void)
{
    HAL_UART_Receive_DMA(&huart2, LightstripBoard_buffer.RX, UART_BUFSIZE);
}

