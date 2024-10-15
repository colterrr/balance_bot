#ifndef _PID_H_
#define _PID_H_

#include "stdint.h"

typedef enum pid_depth_e
{
    PID_SINGLE = 0,
    PID_CASCADE,
}pid_depth;

typedef struct pid_config_s
{
    float Kp;
    float Ki;
    float Kd;
    pid_depth depth;
    float* fdb_p;
    float err_max;
    float output_max;

    uint8_t reverse_flag; //输出反转设置 0正常 1输出反转
}pid_config;


typedef struct pid_s
{
    pid_config config;
    float err[3];
    float output;
    float ref;
}pid;

pid* PID_Create(pid_config p_config);
void PID_calc(pid*);

#endif