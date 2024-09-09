#include "string.h"
#include "stdint.h"
#include "usart.h"

typedef void (uart_rx_func)(uint8_t* pdata, uint16_t len);

#define MAX_BUF_LEN_R 128
#define MAX_BUF_LEN_T 128
#pragma pack(1)
typedef struct BSP_UART_Type_s
{
    uint8_t txbuffer[MAX_BUF_LEN_T];
    uint8_t rxbuffer[MAX_BUF_LEN_R];
    uart_rx_func* user_func;
    UART_HandleTypeDef* huart;
    uint16_t rx_Len;
}BSP_UART_Type;
#pragma pack()

#define MAX_PORT_NUM 2
__section(".buffer_used") BSP_UART_Type uart_port[MAX_PORT_NUM] = {};

/***------初始化------***/

void BSP_UART_Init(void)
{
    uart_port[0].huart = &huart1;
    uart_port[0].rx_Len = 16;
    HAL_DMA_DeInit(huart1.hdmarx);
    HAL_DMA_Init(huart1.hdmarx);
    HAL_UART_Receive_DMA(uart_port[0].huart, uart_port[0].rxbuffer, uart_port[0].rx_Len);
}

/***------接收------***/

void BSP_UART_IRQHandler(UART_HandleTypeDef* huart)
{
    // if ( __HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) ) {
    //     __HAL_UART_CLEAR_IDLEFLAG(huart);
    //     HAL_UART_DMAStop(huart);
    //     uint16_t len = MAX_BUF_LEN_R - __HAL_DMA_GET_COUNTER(huart->hdmarx);
        
    //     for (uint8_t i = 0; i < MAX_PORT_NUM; i++){
    //         if (huart == uart_port[i].huart){
    //             if (uart_port[i].user_func != NULL)
    //                 (*(uart_port[i].user_func))(uart_port[i].rxbuffer, len);
    //             HAL_UART_Receive_IT(huart, uart_port[i].rxbuffer, 16);
    //         }
    //     }
    // }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
    for (uint8_t i = 0; i < MAX_PORT_NUM; i++) {
        if (huart == uart_port[i].huart){
            if (uart_port[i].user_func != NULL) {
                (*(uart_port[i].user_func))(uart_port[i].rxbuffer, uart_port[i].rx_Len);                
            }
            HAL_UART_Receive_DMA(huart, uart_port[i].rxbuffer, uart_port[i].rx_Len);
        }
    }
}

/***------发送------***/

void BSP_UART_send(uint8_t port_index, uint8_t* pdata, uint16_t len)
{
    memcpy(uart_port[port_index].txbuffer, pdata, len);
    HAL_UART_Transmit_IT(uart_port[port_index].huart, uart_port[port_index].txbuffer, len);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{

}

/***------设置callback函数------***/

void BSP_UART_registerfunc(uart_rx_func* user_func, uint8_t port_index)
{
    uart_port[port_index].user_func = user_func;
}
