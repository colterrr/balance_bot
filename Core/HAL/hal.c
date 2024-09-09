#include "hal.h"

#include "BLDC_motor.h"
#include "imu.h"
#include "encoder.h"
#include "xbox_ctrler.h"
#include "watch_dog.h"

void HAL_my_Init()
{
    BLDC_motor_Init();
    Encoder_Init();
    xbox_init();
    //IMU_Init();   
    WatchDog_Init();
}