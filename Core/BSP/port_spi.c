#include "string.h"
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


#if HARDWARE_SPI
BSP_SPI_Type SPI_Port[MAX_SPI_NUM];
#elif SOFTWARE_SPI
software_SPI_Port SPI_Port[MAX_SPI_NUM];
GPIO_TypeDef* GPIOx_SCK;
GPIO_TypeDef* GPIOx_MOSI;
GPIO_TypeDef* GPIOx_MISO;
GPIO_TypeDef* GPIOx_NSS;
uint16_t GPIO_Pin_SCK;
uint16_t GPIO_Pin_MOSI;
uint16_t GPIO_Pin_MISO;
uint16_t GPIO_Pin_NSS;
#endif

void BSP_SPI_Init(void)
{
    #if HARDWARE_SPI
    SPI_Port[0].hspi = &hspi1;

    #elif SOFTWARE_SPI
    SPI_Port[0].IOx_SCK = GPIOA;
    SPI_Port[0].Pin_SCK = GPIO_PIN_5;
    SPI_Port[0].IOx_MISO = GPIOA;
    SPI_Port[0].Pin_MISO = GPIO_PIN_6;
    SPI_Port[0].IOx_MOSI = GPIOA;
    SPI_Port[0].Pin_MOSI = GPIO_PIN_7;


    for (uint8_t i = 0; i < MAX_SPI_NUM; i++){
        //时钟空闲为低电平
        HAL_GPIO_WritePin(SPI_Port[i].IOx_SCK, SPI_Port->Pin_SCK, 0);
    }

    #endif
}

void BSP_SPI_Transmit_Byte(uint8_t port_index, uint8_t* pdata)
{
    #if HARDWARE_SPI
    uint32_t now_time = HAL_GetTick();
    while (__HAL_SPI_GET_FLAG(SPI_Port[port_index].hspi,SPI_FLAG_TXE) == RESET)
        if(HAL_GetTick() - now_time > SPI_WAIT_TIME)
            break;
    __HAL_SPI_CLEAR_FLAG(SPI_Port[port_index].hspi,SPI_FLAG_TXE);
    HAL_SPI_Transmit(SPI_Port[port_index].hspi, pdata, 1, SPI_WAIT_TIME);

    SPI_Port[port_index].hspi->Instance->DR = *pdata;
    #elif SOFTWARE_SPI
    GPIOx_SCK = SPI_Port[port_index].IOx_SCK;
    GPIOx_MOSI = SPI_Port[port_index].IOx_MOSI;
    GPIO_Pin_SCK = SPI_Port[port_index].Pin_SCK;
    GPIO_Pin_MOSI = SPI_Port[port_index].Pin_MOSI;

    uint8_t data = *pdata;
    for (uint8_t i = 0; i < 8; i++){
        L_SCK;
        if (data & 0x80){
            H_MOSI;
        }
        else {
            L_MOSI;
        }
        H_SCK;
        delay_us(1);
        data<<=1;
    }
    #endif
}

void BSP_SPI_Receive(uint8_t port_index, uint8_t* rxbuf, uint8_t len)
{
    #if HARDWARE_SPI
    //HAL_SPI_Receive_IT(SPI_Port[port_index].hspi, rxbuf, len);

    #elif SOFTWARE_SPI
    GPIOx_SCK = SPI_Port[port_index].IOx_SCK;
    GPIOx_MISO = SPI_Port[port_index].IOx_MISO;
    GPIO_Pin_SCK = SPI_Port[port_index].Pin_SCK;
    GPIO_Pin_MISO = SPI_Port[port_index].Pin_MISO;

    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++){
        L_SCK;
        if (READ_MISO);
    }
    
    #endif
}
