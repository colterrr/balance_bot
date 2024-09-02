#include "BLDC_motor_can.h"

#include "cvector.h"
#include "stdlib.h"
#include "string.h"
#include "bsp.h"

cvector* BLDC_MotorCan_ins;

/**
 * @brief 电机传回数据处理
 * @param id 
 * @param pdata 
 * @param len 
 */
void BLDC_MotorCan_Update(uint16_t id, uint8_t* pdata, uint16_t len)
{
    if (len != 8) return; //数据长度不对，舍弃
    for (uint8_t i = 0;i < BLDC_MotorCan_ins->cv_len; i++){
        BLDC_motor_can* obj = *(BLDC_motor_can**)cvector_val_at(BLDC_MotorCan_ins, i);
        if (obj->config.id == id){
            /*
            电机传回的数据报文：
            初始位置 
            位置 4byte   (float32_t)
            角速度 4byte (float32_t)
            */
            obj->pos = *(float*)pdata;
            obj->w = *(float*)(pdata + 4);
        }
    }
}


void BLDC_MotorCan_Init()
{
    BLDC_MotorCan_ins = cvector_create(sizeof(BLDC_motor_can*));
    BSP_FDCAN_RegisterCallback(0, BLDC_MotorCan_Update);
}

BLDC_motor_can* BLDC_MotorCan_Create(BLDC_motor_can_config config)
{
    BLDC_motor_can* obj = malloc(sizeof(BLDC_motor_can));
    memset(obj, 0, sizeof(BLDC_motor_can));
    obj->config = config;
    cvector_pushback(BLDC_MotorCan_ins, &obj);
    return obj;
}

/**
 * @brief 电机输出期望值发送
 */
void BLDC_MotorCan_Send()
{
    for (uint8_t i = 0;i < BLDC_MotorCan_ins->cv_len; i++){
        BLDC_motor_can* obj = *(BLDC_motor_can**)cvector_val_at(BLDC_MotorCan_ins, i);
        static uint8_t send_data[5];
        /*
        发送电机的控制报文：
        模式 1byte 0力矩 1速度 2位置
        目标值 4byte (float_32t)
        */        
        send_data[0] = obj->mode;
        *(float*)(send_data + 1) = obj->ref;
        BSP_FDCAN_Transmit(send_data, 5, obj->config.id);
    }

}