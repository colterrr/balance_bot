#include "bsp.h"

void BSP_Init()
{
    BSP_GPIO_Init();
    BSP_IIC_Init();
    BSP_PWM_Init();
    //BSP_ADC_Init();
    BSP_CAN_Init();
}