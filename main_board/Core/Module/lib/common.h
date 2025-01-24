#ifndef __COMMON_H
#define __COMMON_H
#include "stdint.h"
#define pi 3.1415926f
#define RAD2DEG (180.0f / pi)
#define DEG2RAD (pi / 180.0f)
// // 取最大值
// #define Max(a, b) a > b ? a : b
// // 取最小值
// #define Min(a, b) a < b ? a : b
// // 双端限幅 a<x<b
// #define Mid(x, a, b) Max(Min(x,b),a)
// // 0的近似值
// #define MINI 1e-3

typedef enum Commu_sta_e
{
    COMMU_ON = 0,
    COMMU_ERR,
}Commu_sta;

float invSqrt(float x);
int sgn(int x);
int fsgn(float x);
float sgn_like(float x, float d);

#define max(a,b) (a)>(b)?(a):(b)
#define min(a,b) (a)<(b)?(a):(b)
#define bound(mn, a, mx) (a)<(mn)?(mn):((a)>(mx)?(mx):(a))

void delay_ms(uint16_t ms);
void delay_us(uint32_t us);

#endif