#include "encoder.h"
#include "cvector.h"
#include "bsp.h"

#define RADtoDEG 57.29578

cvector* encoder_ins;

void Encoder_Init()
{
    encoder_ins = cvector_create(sizeof(encoder));
}

void Encoder_Update()
{
    for (uint8_t i = 0; i < encoder_ins->cv_len; i++){
        encoder* obj = *(encoder**)cvector_val_at(encoder_ins, i);
        //粗略数据获取
        uint8_t reg = ANGLE_REG;
        uint8_t buf[2] = {};
        Master_Complex(obj->iic_port, &reg, 1, buf, 2);
        obj->raw_angle = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
        //判断是否为初始位置数据
        if (!obj->init_flag){
            obj->last_angle = obj->raw_angle;
            obj->init_deg = (float)(obj->raw_angle) /4096 *360;   
            obj->angle_deg = obj->init_deg;
            obj->init_flag = 1; 
        }
        //具体数据计算
        float angle_delta = (float)(obj->raw_angle - obj->last_angle) /4096 *360;
        float abs_delta;
        if (angle_delta < -180) 
            abs_delta = angle_delta + 360;
        else if (angle_delta > 180) 
            abs_delta = angle_delta - 360;
        else                        
            abs_delta = angle_delta;
        
        obj->angle_deg += abs_delta;
        obj->last_angle = obj->raw_angle;
        obj->angle_rad = obj->angle_deg / RADtoDEG;

        if (obj->p_angle_queue->cq_len == QUEUE_NUM){
            float* last = circular_queue_pop(obj->p_angle_queue);
            obj->delta_sum -= *last;
        }
        circular_queue_push(obj->p_angle_queue, &abs_delta);
        obj->delta_sum += abs_delta;
        obj->w_deg = obj->delta_sum * 200;
        obj->w_rad = obj->w_deg / RADtoDEG;        
    }

}

encoder* Encoder_Create(uint8_t iic_port, void* IOx_CL, uint16_t Pin_CL, void* IOx_DA, uint16_t Pin_DA)
{
    encoder* obj = malloc(sizeof(encoder));
    memset(obj, 0 ,sizeof(encoder));
    BSP_IIC_setpara(iic_port, IOx_CL, Pin_CL, IOx_DA, Pin_DA, ENCODER_ADDR);
    obj->iic_port = iic_port;
    obj->p_angle_queue = create_circular_queue(sizeof(float), QUEUE_NUM);
    cvector_pushback(encoder_ins, &obj);
    return obj;
}
