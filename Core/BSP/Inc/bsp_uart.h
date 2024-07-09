#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#include "stdint.h"
#include "usart.h"

#define MAX_BUF_LEN_R 512
#define MAX_BUF_LEN_T 128
#define MAX_PORT_NUM 2

typedef void (rx_handle_func)(UART_HandleTypeDef* huart, uint8_t* rxbuffer, uint16_t len);

#pragma pack(1)
typedef struct BSP_UART_Type_s
{
    uint8_t txbuffer[MAX_BUF_LEN_T];
    uint8_t rxbuffer[MAX_BUF_LEN_R];
    rx_handle_func* user_func;
    UART_HandleTypeDef* huart;
}BSP_UART_Type;
#pragma pack()

void BSP_UART_Init(void);
void BSP_UART_send(uint8_t port_index, uint8_t* pdata, uint16_t len);
void BSP_UART_IRQHandler(UART_HandleTypeDef* huart);

void BSP_UART_registerfunc(rx_handle_func* user_func, uint8_t port_index);

#endif