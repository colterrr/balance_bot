#include "bsp_pwm.h"

BSP_PWM_Type PWM_Port[PWM_MAX_NUM];

void BSP_PWM_Init()
{
    PWM_Port[0].htim = &htim1;
    PWM_Port[0].channel = TIM_CHANNEL_1;
    PWM_Port[1].htim = &htim1;
    PWM_Port[1].channel = TIM_CHANNEL_2;
    PWM_Port[2].htim = &htim1;
    PWM_Port[2].channel = TIM_CHANNEL_3;
}

void BSP_PWM_SetARR(uint8_t index, uint16_t ARR_val)
{
    __HAL_TIM_SetAutoreload(PWM_Port[index].htim, ARR_val);
}
void BSP_PWM_SetCCR(uint8_t index, uint16_t CCR_val)
{
    __HAL_TIM_SetCompare(PWM_Port[index].htim, PWM_Port[index].channel, CCR_val);
}

void BSP_PWM_Start(uint8_t index)
{
    HAL_TIM_PWM_Start(PWM_Port[index].htim, PWM_Port[index].channel);
}
void BSP_PWM_Stop(uint8_t index)
{
    HAL_TIM_PWM_Start(PWM_Port[index].htim, PWM_Port[index].channel);
}