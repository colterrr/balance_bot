#ifndef _FOC_H_
#define _FOC_H_

#include "stdint.h"
#include "arm_math.h"

#define SQRT_3 1.73205

typedef struct vector_2len_dq_s
{
    float32_t q;
    float32_t d;
}vector_2len_dq;

typedef struct vector_2len_ab_s
{
    float32_t alpha;
    float32_t beta;
}vector_2len_ab;

typedef struct vector_3len_s
{
    float32_t a;
    float32_t b;
    float32_t c;
}vector_3len;

typedef struct foc_s
{
    //电压开环
    uint16_t* angle_p;
    //电流闭环
}foc;

void anti_park(vector_2len_dq voltage_tar, float theta, vector_2len_ab* ans);
void anti_park_clark(vector_2len_dq voltage_tar, float theta, vector_3len* ans);
void park_clark(vector_3len currnet_now, float theta, vector_2len_dq* ans);
#endif