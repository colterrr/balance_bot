#include "encoder.h"
#include "cvector.h"
#include "bsp_iic.h"

cvector* encoder_ins;

void Encoder_Init()
{
    encoder_ins = cvector_create(sizeof(encoder));
}

void Encoder_Update()
{
    for (uint8_t i = 0; i < encoder_ins->cv_len; i++){
        encoder* obj = *(encoder**)cvector_val_at(encoder_ins, i);
        //初始数据获取
        uint8_t reg = RAW_ANGLE;
        uint8_t buf[2] = {};
        Master_Complex(obj->iic_port, &reg, 1, buf, 2);
        obj->raw_angle = ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
        obj->raw_angle -= 32768;
        //具体数据计算
        
    }
    
}

encoder* Encoder_Create(uint8_t iic_port, void* IOx_CL, uint16_t Pin_CL, void* IOx_DA, uint16_t Pin_DA)
{
    encoder* obj = malloc(sizeof(encoder));
    memset(obj, 0 ,sizeof(encoder));
    BSP_IIC_setpara(iic_port, IOx_CL, Pin_CL, IOx_DA, Pin_DA, ENCODER_ADDR);
    obj->iic_port = iic_port;
    cvector_pushback(encoder_ins, &obj);
    return obj;
}
