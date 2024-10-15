#include "myfunc.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tim.h"

// //整型转字符，写入指定连续空间，返回长度
// uint8_t Uint_to_Str(uint32_t num, uint8_t* place)
// {
//     uint16_t k,i,tem;
//     //算出i位数
//     for (k = 10, i = 1; ; i++, k *= 10){
//         if (num / k == 0)break;
//     }
//     for (k = i; i > 0; i--){
//         tem = p(10, i - 1);
//         place[k - i] = num / tem + '0';
//         num -= (place[k - i] - '0') * tem;
//     }
//     return k;
// }

// //指定数量字符转整型，自带合法检测
// uint32_t Str_to_uint(uint8_t* pdata, uint8_t len)
// {
//     uint32_t num = 0;
//     uint8_t k = 0;
//     for(k = 1; len > 0; len--, k *= 10){
//         num += (pdata[len-1] - '0') * k;
//     }
//     return num;
// }

/**
 * @brief 粗略毫秒延时
 * @param ms 
 */
void delay_ms(uint16_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

/**
 * @brief 精确微秒延时
 * @param us 
 */
void delay_us(uint32_t us)
{
    __HAL_TIM_DISABLE(&htim4);
    __HAL_TIM_SetCounter(&htim4, 0);
    __HAL_TIM_ENABLE(&htim4);
    while (__HAL_TIM_GET_COUNTER(&htim4) < us);
    
}
