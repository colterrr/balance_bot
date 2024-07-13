#ifndef _MYFUNC_H_
#define _MYFUNC_H_
#include "stdint.h"

#define max(a,b) a>b?a:b
#define min(a,b) a<b?a:b

void delay_ms(uint16_t ms);
void delay_us(uint32_t us);

void delay_ms_sss(uint16_t ms);

#endif