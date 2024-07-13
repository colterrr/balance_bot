#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_

#include "stdint.h"
#include "tim.h"

//htim5 config:
//PSC 240-1
//ARR 2^32-1

//htim1 config: 
//PSC 2-1  
//ARR 5000-1

#define PWM_MAX_NUM 7

typedef struct BSP_PWM_Type_s
{
    uint8_t channel;
    TIM_HandleTypeDef* htim;
}BSP_PWM_Type;

void BSP_PWM_Init();

void BSP_PWM_SetARR(uint8_t index, uint16_t ARR_val);
void BSP_PWM_SetCCR(uint8_t index, uint16_t CCR_val);
void BSP_PWM_Setduty(uint8_t index, float duty);
void BSP_PWM_Start(uint8_t index);
void BSP_PWM_Stop(uint8_t index);

#endif