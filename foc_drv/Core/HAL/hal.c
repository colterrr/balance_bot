#include "hal.h"

#include "BLDC_motor.h"
#include "encoder.h"

void HAL_my_Init()
{
    BLDC_motor_Init();
    Encoder_Init();
}