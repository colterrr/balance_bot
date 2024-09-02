#ifndef _BLDC_MOTOR_CAN_H_
#define _BLDC_MOTOR_CAN_H_

#include "stdint.h"

typedef enum
{
    TORQUE_MODE = 0,   //力矩输出模式
    SPEED_MODE,        //速度控制模式
    POSITION_MODE      //位置控制模式
}Control_mode;

typedef struct
{
    uint16_t id;
}BLDC_motor_can_config;

typedef struct
{
    BLDC_motor_can_config config;
    uint8_t EN;
    Control_mode mode;

    float ref; //期望值
    float pos;
    float w;

}BLDC_motor_can;


void BLDC_MotorCan_Init();
BLDC_motor_can* BLDC_MotorCan_Create(BLDC_motor_can_config config);
void BLDC_MotorCan_Update(uint16_t id, uint8_t* pdata, uint16_t len);

#endif