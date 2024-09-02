#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "stdint.h"

#include "circular_queue.h"

#define ENCODER_ADDR 0x36
#define RAW_ANGLE 0x0C
#define QUEUE_NUM 5 //计算角速度队列长度

typedef struct
{
    uint8_t init_flag;
    uint16_t init_angle; //初始角度

    uint8_t iic_port;

    uint16_t raw_angle;
    uint16_t last_angle;
    
    float delta_sum;
    float angle_deg;
    float angle_rad;
    float w_deg;
    float w_rad;
    float n;

    circular_queue* p_angle_queue;
}encoder;

void Encoder_Init();
void Encoder_Update();
encoder* Encoder_Create(uint8_t iic_port, void* IOx_CL, uint16_t Pin_CL, void* IOx_DA, uint16_t Pin_DA);

#endif