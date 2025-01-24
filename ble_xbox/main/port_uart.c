#include "port_uart.h"
#include "driver/uart.h"
#include "string.h"

#define TERMINAL_CHECK 1

void uart_init()
{
    //通信参数设置
    uart_config_t uart_config = {
    .baud_rate = UART_BAUD_RATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    //通信驱动安装
    //QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUF_SIZE, UART_BUF_SIZE, 0, NULL, 0));
    //通信管脚设置
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

int uart_send(uint8_t* pdata, uint16_t len)
{
    const int txBytes = uart_write_bytes(UART_PORT_NUM, pdata, len);
    #if TERMINAL_CHECK
    printf("Wrote %d bytes", txBytes);
    #endif
    return txBytes;
}

int uart_send_str(char* str)
{
    const int len = strlen(str);
    const int txBytes = uart_write_bytes(UART_PORT_NUM, str, len);
    #if TERMINAL_CHECK
    printf("Wrote %d bytes", txBytes);
    #endif
    return txBytes;
}