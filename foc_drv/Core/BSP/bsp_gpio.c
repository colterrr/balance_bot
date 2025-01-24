#include "gpio.h"
#include "main.h"
#include "stdint.h"

typedef struct gpio_port_s
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
}gpio_port;

#define GPIO_MAX_NUM 5
static gpio_port ports[GPIO_MAX_NUM];

void BSP_GPIO_out(uint8_t index, uint8_t output)
{
    HAL_GPIO_WritePin(ports[index].GPIOx, ports[index].GPIO_Pin, output);
}

uint8_t BSP_GPIO_in(uint8_t index)
{
    return HAL_GPIO_ReadPin(ports[index].GPIOx, ports[index].GPIO_Pin);
}

// 编号所对应功能的宏
#define ENCODER_DIR_PORT 0
#define MOTOR_PORT 1
#define FAULT_PORT 2
void BSP_GPIO_Init()
{
    ports[ENCODER_DIR_PORT].GPIOx = GPIOB;
    ports[ENCODER_DIR_PORT].GPIO_Pin = GPIO_PIN_4;
    ports[MOTOR_PORT].GPIOx = GPIOA;
    ports[MOTOR_PORT].GPIO_Pin = GPIO_PIN_9;
    ports[FAULT_PORT].GPIOx = GPIOB;
    ports[FAULT_PORT].GPIO_Pin = GPIO_PIN_12;
}