#ifndef _ENCODER_H_
#define _ENCODER_H_
#include "stdint.h"

#define ENCODER_ADDR 0x36
#define RAW_ANGLE 0x0C

typedef struct encoder_s
{
    uint16_t raw_angle;
    float angle;
    uint8_t iic_port;
}encoder;

void Encoder_Init();
void Encoder_Update();
encoder* Encoder_Create(uint8_t iic_port, void* IOx_CL, uint16_t Pin_CL, void* IOx_DA, uint16_t Pin_DA);

#endif