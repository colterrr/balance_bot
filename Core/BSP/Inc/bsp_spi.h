#ifndef _BSP_SPI_H_
#define _BSP_SPI_H_

#include "stdint.h"
#include "main.h"

#define HARDWARE_SPI 0
#define SOFTWARE_SPI !HARDWARE_SPI

#define MAX_SPI_NUM 1 //使用SPI的设备数量
#define SPI_WAIT_TIME 1 //阻塞模式等待时间 单位ms

#define L_SCK HAL_GPIO_WritePin(GPIOx_SCK, GPIO_Pin_SCK, GPIO_PIN_RESET)
#define H_SCK HAL_GPIO_WritePin(GPIOx_SCK, GPIO_Pin_SCK, GPIO_PIN_SET)
#define L_MOSI HAL_GPIO_WritePin(GPIOx_MOSI, GPIO_Pin_MOSI, GPIO_PIN_RESET)
#define H_MOSI HAL_GPIO_WritePin(GPIOx_MOSI, GPIO_Pin_MOSI, GPIO_PIN_SET)
#define READ_MISO HAL_GPIO_ReadPin(GPIOx_SCK, GPIO_Pin_MISO)

typedef struct BSP_SPI_Type_s
{
    
}BSP_SPI_Type;

//软件SPI默认：SCK空闲为低、奇数采样
//不使用总线式布局，使用点对点式布局
typedef struct software_SPI_Port_s
{
    GPIO_TypeDef* IOx_SCK;
    GPIO_TypeDef* IOx_MOSI;
    GPIO_TypeDef* IOx_MISO;
    GPIO_TypeDef* IOx_NSS;
    uint16_t Pin_SCK;
    uint16_t Pin_MOSI;
    uint16_t Pin_MISO;
    uint16_t Pin_NSS;
}software_SPI_Port;


void BSP_SPI_Transmit_Byte(uint8_t port_index, uint8_t* pdata);
void BSP_SPI_Init(void);
void BSP_SPI_Receive(uint8_t port_index, uint8_t* rxbuf, uint8_t len);
#endif