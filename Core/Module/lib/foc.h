#ifndef _FOC_H_
#define _FOC_H_

#include "stdint.h"
#include "arm_math.h"

#include "pid.h"

#define SQRT_3 1.73205

typedef enum ctrl_type_e
{
    CURRENT_CLOSE = 0, //电流闭环
    VOLTAGE_OPEN       //电压开环 
}ctrl_type;


typedef struct vector_dq_s
{
    float32_t q;
    float32_t d;
}vector_dq;

typedef struct vector_ab_s
{
    float32_t alpha;
    float32_t beta;
}vector_ab;

typedef struct vector_abc_s
{
    float32_t a;
    float32_t b;
    float32_t c;
}vector_abc;

typedef struct foc_config_s
{
    ctrl_type foc_type;
    uint8_t index_a; //a相pwm索引号
    uint8_t index_b; //b相pwm索引号
    uint8_t index_c; //c相pwm索引号
}foc_config;


typedef struct foc_s
{
    //通用参数
    foc_config config; //配置结构体
    float angle_dq; //dq系电角度，需要外部更新
    float angle_ab; //ab系电角度，需要外部更新
    float d_theta; //dq系和ab系夹角

    float a;
    float b;
    //电压开环

    //电流闭环
    pid* cur;
}foc;

void FOC_Init();
void FOC_Calc(foc* obj, float ref);
foc* FOC_Create(foc_config config);

uint8_t SVPWM(foc* obj, vector_ab ab);
#endif