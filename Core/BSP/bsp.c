#include "bsp.h"

#include "bsp_pwm.h"
#include "bsp_gpio.h"
#include "bsp_iic.h"
#include "bsp_uart.h"
#include "bsp_spi.h"
#include "bsp_adc.h"

void BSP_Init()
{
    BSP_GPIO_Init();
    BSP_IIC_Init();
    BSP_PWM_Init();
}