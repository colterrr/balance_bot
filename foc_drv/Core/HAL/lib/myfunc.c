#include "myfunc.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tim.h"

/**
 * @brief 粗略毫秒延时
 * @param ms 
 */
void delay_ms(uint16_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/**
 * @brief 精确微秒延时
 * @param us 
 */
void delay_us(uint32_t us)
{
    __HAL_TIM_DISABLE(&htim2);
    __HAL_TIM_SetCounter(&htim2, 0);
    __HAL_TIM_ENABLE(&htim2);
    while (__HAL_TIM_GET_COUNTER(&htim2) < us);
    
}
