#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include "encoder.h"
#include "BLDC_motor.h"
#include "imu.h"

typedef struct instance_s
{
    BLDC_motor* motor1;
    imu_atk* imu;

    // 调试用
    float theta;
    uint8_t zone;
    float a;
    float b;
}instance;


void Instance_init();
void Instance_Update();

#endif