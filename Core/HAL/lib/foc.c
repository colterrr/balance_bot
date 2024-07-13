#include "foc.h"

#include "arm_math.h"
#include "stdlib.h"
#include "bsp_pwm.h"

// void anti_park_clark(vector_dq voltage_tar, float theta, vector_abc* ans)
// {
//     float32_t v_alpha = voltage_tar.d * arm_cos_f32(theta) - voltage_tar.q * arm_sin_f32(theta);
//     float32_t v_beta  = voltage_tar.d * arm_sin_f32(theta) + voltage_tar.q * arm_cos_f32(theta);

//     ans->a = v_alpha;
//     ans->b = v_alpha *-0.5 +  v_beta *SQRT_3 /2;
//     ans->c = v_alpha *-0.5 - v_beta *SQRT_3 /2;
// }

#define pCCR3 (1-p1-p2)/2
#define pCCR2 (1+p1-p2)/2
#define pCCR1 (1+p1+p2)/2

void anti_park(vector_dq voltage_tar, float theta, vector_ab* ans)
{
    ans->alpha = voltage_tar.d * cos(theta) - voltage_tar.q * sin(theta);
    ans->beta  = voltage_tar.d * sin(theta) + voltage_tar.q * cos(theta);
}

void park_clark(vector_abc currnet_now, float theta, vector_dq* ans)
{
    
}   

foc* FOC_Create(foc_config config)
{
    foc* obj = malloc(sizeof(foc));
    obj->config = config;
    return obj;
}
/**
 * @brief  进行foc控制
 * @param obj foc对象
 * @param ref 目标值
 */
void FOC_Calc(foc* obj, float ref)
{
    obj->d_theta = obj->angle_dq - obj->angle_ab;
    if (obj->config.foc_type == VOLTAGE_OPEN){
        vector_ab ans;
        vector_dq ref_dq;
        ref_dq.d = 0;
        ref_dq.q = ref;
        anti_park(ref_dq, obj->d_theta, &ans);
        obj->a = ans.alpha;
        obj->b = ans.beta;
        SVPWM(obj, ans);
    }
    else if (obj->config.foc_type == CURRENT_CLOSE){
        
    }
}

/**
 * @brief 产生svpwm
 * @param obj foc对象
 * @param ab ab轴要求的电压与电压最大值的比例
 */
uint8_t SVPWM(foc* obj, vector_ab ab)
{
    uint8_t zone;
    if (ab.beta > 0){
        if (ab.beta /ab.alpha > SQRT_3 || ab.beta /ab.alpha < -SQRT_3){
            zone = 2;
            //区域2
            float p1 = ab.beta /SQRT_3 + ab.alpha;
            float p2 = ab.beta /SQRT_3 - ab.alpha;
            //CCR3-C CCR2-A CCR1-B
            BSP_PWM_Setduty(obj->config.index_c, pCCR3);
            BSP_PWM_Setduty(obj->config.index_a, pCCR2);
            BSP_PWM_Setduty(obj->config.index_b, pCCR1);
        }
        else if (ab.alpha > 0){
            zone = 1;
            //区域1
            float p1 = ab.beta *2 /SQRT_3;
            float p2 = ab.alpha - ab.beta /SQRT_3; 
            //CCR3-C CCR2-B CCR1-A
            BSP_PWM_Setduty(obj->config.index_c, pCCR3);
            BSP_PWM_Setduty(obj->config.index_b, pCCR2);
            BSP_PWM_Setduty(obj->config.index_a, pCCR1);
        }
        else {
            zone = 3;
            //区域3
            float p1 = - ab.beta /SQRT_3 - ab.alpha; 
            float p2 = ab.beta *2 /SQRT_3; 
            //CCR3-A CCR2-C CCR1-B
            BSP_PWM_Setduty(obj->config.index_a, pCCR3);
            BSP_PWM_Setduty(obj->config.index_c, pCCR2);
            BSP_PWM_Setduty(obj->config.index_b, pCCR1);
        }
    }
    else {
        if (ab.beta / ab.alpha > SQRT_3 || ab.beta / ab.alpha < -SQRT_3){
            zone = 5;
            //区域5
            float p1 = - ab.beta /SQRT_3 + ab.alpha;
            float p2 = - ab.beta /SQRT_3 - ab.alpha;
            //CCR3-B CCR2-A CCR1-C
            BSP_PWM_Setduty(obj->config.index_b, pCCR3);
            BSP_PWM_Setduty(obj->config.index_a, pCCR2);
            BSP_PWM_Setduty(obj->config.index_c, pCCR1);
        }
        else if (ab.alpha > 0){
            zone = 6;
            //区域6
            float p1 = - ab.beta *2 /SQRT_3;
            float p2 = ab.alpha + ab.beta /SQRT_3; 
            //CCR3-B CCR2-C CCR1-A
            BSP_PWM_Setduty(obj->config.index_b, pCCR3);
            BSP_PWM_Setduty(obj->config.index_c, pCCR2);
            BSP_PWM_Setduty(obj->config.index_a, pCCR1);            
        }
        else {
            zone = 4;
            //区域4
            float p1 = ab.beta /SQRT_3 - ab.alpha; 
            float p2 = - ab.beta *2 /SQRT_3; 
            //CCR3-A CCR2-B CCR1-C
            BSP_PWM_Setduty(obj->config.index_a, pCCR3);
            BSP_PWM_Setduty(obj->config.index_b, pCCR2);
            BSP_PWM_Setduty(obj->config.index_c, pCCR1);
        }        
    }
    return zone;
}