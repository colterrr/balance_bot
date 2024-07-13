#include "pid.h"
#include "string.h"
#include "stdlib.h"

pid* PID_Create(pid_config pid_config)
{
    pid* obj = malloc(sizeof(pid));
    memset(obj, 0, sizeof(pid));
    obj->config = pid_config;
    return obj;
}

void PID_calc(pid* pid)
{
    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];
    pid->err[0] = pid->ref - *pid->config.fdb_p;

    pid->output += pid->config.Kp * (pid->err[0] - pid->err[1]) \
                 + pid->config.Ki * pid->err[0] \
                 + pid->config.Kd * (pid->err[0] - 2*pid->err[1] + pid->err[2]);

    if (pid->output > pid->config.output_max) pid->output = pid->config.output_max;
}