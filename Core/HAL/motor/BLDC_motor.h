#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "foc.h"
#include "pid.h"
#include "encoder.h"

typedef enum
{
    TORQUE_MODE  = 0, //位置控制模式
    SPEED_MODE,        //速度控制模式
    POSITION_MODE      //力矩输出模式
}Control_mode;

typedef struct
{
    Control_mode mode;
    uint8_t en_port;
    foc_config foc_cfg;
    pid_config config_pos;
    pid_config config_spd;

    //电机参数
    uint8_t pole_pair;   //极对数
    float torque_cons; //转矩常数
    float voltage_max; //最大电压
}BLDC_motor_config;


typedef struct BLDC_motor_s
{
    BLDC_motor_config config;
    foc* motor_foc;
    uint8_t EN;
    uint8_t init_flag;

    encoder* motor_encoder;
    pid* pid_pos;
    pid* pid_spd;

    float ref;  //目标值，三种模式共用此变量，注意数量级和单位 rad - rad/s - N/m
    float pos;
    float w;
    float output;

    //调试用
    vector_ab ref_ab;
    float theta;
}BLDC_motor;

BLDC_motor* BLDC_motor_Create(BLDC_motor_config config);
void BLDC_motor_Init();
void BLDC_motor_Update();


#endif