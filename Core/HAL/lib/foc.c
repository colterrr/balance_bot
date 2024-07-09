#include "foc.h"
#include "arm_math.h"

void anti_park_clark(vector_2len_dq voltage_tar, float theta, vector_3len* ans)
{
    float32_t v_alpha = voltage_tar.d * arm_cos_f32(theta) - voltage_tar.q * arm_sin_f32(theta);
    float32_t v_beta  = voltage_tar.d * arm_sin_f32(theta) + voltage_tar.q * arm_cos_f32(theta);

    ans->a = v_alpha;
    ans->b = v_alpha *-0.5 +  v_beta *SQRT_3 /2;
    ans->c = v_alpha *-0.5 - v_beta *SQRT_3 /2;
}

void anti_park(vector_2len_dq voltage_tar, float theta, vector_2len_ab* ans)
{
    ans->alpha = voltage_tar.d * arm_cos_f32(theta) - voltage_tar.q * arm_sin_f32(theta);
    ans->beta  = voltage_tar.d * arm_sin_f32(theta) + voltage_tar.q * arm_cos_f32(theta);
}

void park_clark(vector_3len currnet_now, float theta, vector_2len_dq* ans)
{
    
}   

//电流闭环的SVPWM
vector_3len SVPWM(vector_2len_ab voltage_tar, uint8_t zone)
{
    switch (zone)
    {
    case 1:
        break;

    case 2:
        break;

    case 3:
        break;

    case 4:
        break;

    case 5:
        break;
    
    case 6:
        break;
    
    default:
        break;
    }
}

//电压开环的SVPWM
vector_3len SVPWM_open(uint16_t voltage_tar, float theta)
{
    
}