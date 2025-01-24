#ifndef _PORT_UART_H_
#define _PORT_UART_H_

#include "stdint.h"

#define UART_PORT_NUM      UART_NUM_1        // 使用 UART1
#define UART_BAUD_RATE     115200            // 波特率
#define UART_TX_PIN        17                // TX 引脚
#define UART_RX_PIN        16                // RX 引脚
#define UART_BUF_SIZE      (1024 * 2)            // 缓冲区大小

void uart_init();
int uart_send(uint8_t* pdata, uint16_t len);
int uart_send_str(char* str);

#endif