#include "bsp_uart.h"
#include "string.h"

BSP_UART_Type uart_port[MAX_PORT_NUM] = {};

/***------初始化------***/

void BSP_UART_Init(void)
{
    uart_port[0].huart = &huart1;
    for (uint8_t i = 0; i < MAX_PORT_NUM; i++){
        __HAL_UART_ENABLE_IT(uart_port[i].huart, UART_IT_IDLE);
        HAL_UART_Receive_DMA(uart_port[i].huart, uart_port[i].rxbuffer, MAX_BUF_LEN_R);
        uart_port[i].user_func = NULL;
    }
}

/***------接收------***/

void BSP_UART_IDLECallback(UART_HandleTypeDef* huart)
{
    if (0x00 != __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)){
        __HAL_UART_CLEAR_IDLEFLAG(huart);
        HAL_UART_DMAStop(huart);
        uint16_t len = MAX_BUF_LEN_R - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        
        for (uint8_t i = 0; i < MAX_PORT_NUM; i++){
            if (huart == uart_port[i].huart){
                if(uart_port[i].user_func != NULL)
                    (*(uart_port[i].user_func))(huart, uart_port[i].rxbuffer, len);
                HAL_UART_Receive_DMA(huart, uart_port[i].rxbuffer, MAX_BUF_LEN_R);
            }
        }
    }
}

void BSP_UART_IRQHandler(UART_HandleTypeDef* huart)
{
    BSP_UART_IDLECallback(huart);
}

/***------发送------***/

void BSP_UART_send(uint8_t port_index, uint8_t* pdata, uint16_t len)
{
    memcpy(uart_port[port_index].txbuffer, pdata, len);
    HAL_UART_Transmit_IT(uart_port[port_index].huart, uart_port[port_index].txbuffer, len);
}

void BSP_UART_Txcpltcallback(UART_HandleTypeDef* huart)
{
    
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    BSP_UART_Txcpltcallback(huart);
}

/***------设置callback函数------***/

void BSP_UART_registerfunc(rx_handle_func* user_func, uint8_t port_index)
{
    uart_port[port_index].user_func = user_func;
}
