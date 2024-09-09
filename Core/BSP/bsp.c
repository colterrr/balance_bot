#include "bsp.h"

void BSP_Init()
{
    BSP_GPIO_Init();
    BSP_IIC_Init();
    BSP_PWM_Init();
    BSP_UART_Init();
}