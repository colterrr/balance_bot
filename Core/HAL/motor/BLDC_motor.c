#include "BLDC_motor.h"

#include "stdlib.h"
#include "string.h"
#include "cvector.h"
#include "myfunc.h"
#include "bsp_gpio.h"

cvector* BLDC_motor_ins;

BLDC_motor* BLDC_motor_Create(BLDC_motor_config config)
{
    BLDC_motor* obj = malloc(sizeof(BLDC_motor));
    memset(obj, 0, sizeof(BLDC_motor));
    obj->config = config;
    obj->motor_foc = FOC_Create(config.foc_cfg);
    obj->pid_pos = PID_Create(config.config_pos);
    obj->pid_spd = PID_Create(config.config_spd);
    cvector_pushback(BLDC_motor_ins, &obj);
    return obj;
}

void BLDC_motor_Init()
{
    BLDC_motor_ins = cvector_create(sizeof(BLDC_motor));
}

void BLDC_motor_calc(BLDC_motor* obj)
{
    //不管电机是否允许工作，始终更新编码器角度值到相关结构成员
    obj->pos = obj->motor_encoder->angle_rad;
    obj->w = obj->motor_encoder->w_rad;

    if (obj->EN == 0) {
        BSP_GPIO_out(obj->config.en_port, 0);
        return;
    }
    else {
       BSP_GPIO_out(obj->config.en_port, 1);
    }

    //输出计算
    if (obj->config.mode == SPEED_MODE){
        obj->pid_spd->ref = obj->ref;
        PID_calc(obj->pid_spd);
        obj->output = obj->pid_spd->output;
    }
    else if (obj->config.mode == POSITION_MODE){
        obj->pid_pos->ref = obj->ref;
        PID_calc(obj->pid_pos);
        obj->pid_spd->ref = obj->pid_pos->output;
        PID_calc(obj->pid_spd);
        obj->output = obj->pid_spd->output;
    }
    else /*即obj->config.mode == TORQUE_MODE*/ {
        obj->output = obj->ref;
    }

    if (obj->config.foc_cfg.foc_type == CURRENT_CLOSE){
        ;
    }
    else if (obj->config.foc_cfg.foc_type == VOLTAGE_OPEN){
        //限制一下最大输出，为最大电压的SQRT(3)/2倍
        obj->output = min(obj->config.voltage_max * SQRT_3 / 2, obj->output) / obj->config.voltage_max;
    }

    obj->motor_foc->angle_dq = obj->pos * obj->config.pole_pair; //更新dq系电角度
    FOC_Calc(obj->motor_foc, obj->output);
}

void BLDC_motor_cali(BLDC_motor* obj)
{
    BSP_GPIO_out(obj->config.en_port, 1);   

    vector_ab ab;
    ab.alpha = 0.5;
    ab.beta = 0;
    SVPWM(obj->motor_foc, ab);
    delay_ms_sss(500);
    obj->motor_foc->angle_ab = obj->motor_encoder->angle_rad * obj->config.pole_pair;//给恒定电压移动到初始电角度，即ab轴电角度 
    ab.alpha = 0;
    ab.beta = 0;
    SVPWM(obj->motor_foc, ab);

    BSP_GPIO_out(obj->config.en_port, 0);    
}

void BLDC_motor_Update()
{
    for (uint8_t i = 0; i < BLDC_motor_ins->cv_len; i++){
        BLDC_motor* obj = *(BLDC_motor**)cvector_val_at(BLDC_motor_ins, i);
        if (!obj->motor_encoder->init_flag) return; //编码器没有校准，退出
        if (!obj->init_flag){
            BLDC_motor_cali(obj); //启动前转子校准
            obj->init_flag = 1;
        }
        else {
            BLDC_motor_calc(obj);            
        }
    }
}