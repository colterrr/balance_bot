#include "bsp_gpio.h"
#include "gpio.h"

#define GPIO_MAX_NUM 1

typedef struct gpio_port_s
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin
}gpio_port;

gpio_port ports[GPIO_MAX_NUM];

void BSP_GPIO_out(uint8_t index, uint8_t output)
{
    HAL_GPIO_WritePin(ports[index].GPIO_Pin, ports[index].GPIOx, output);
}

uint8_t BSP_GPIO_in(uint8_t index)
{
    return HAL_GPIO_ReadPin(ports[index].GPIO_Pin, ports[index].GPIOx);
}

void BSP_GPIO_Init()
{
    ports[0].GPIOx = GPIOD;
    ports[0].GPIO_Pin = GPIO_PIN_5;
}