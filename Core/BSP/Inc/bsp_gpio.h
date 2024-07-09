#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#include "main.h"
#include  "stdint.h"

void BSP_GPIO_out(uint8_t index, uint8_t output);
uint8_t BSP_GPIO_in(uint8_t index);
void BSP_GPIO_Init();

#endif