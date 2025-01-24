#include "module.h"

#include "BLDC_motor_can.h"
#include "imu.h"
#include "encoder.h"
#include "xbox_ctrler.h"
#include "watch_dog.h"

void Module_Init()
{    
    WatchDog_Init();
    BLDC_MotorCan_Init();
    Encoder_Init();
    xbox_init();
    IMU_Init();   
}