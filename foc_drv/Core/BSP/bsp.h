#ifndef _BSP_H_
#define _BSP_H_

#include "stdint.h"
#include "main.h"

void BSP_Init();

// ***
// PWM
// ***
void BSP_PWM_Init();
void BSP_PWM_SetARR(uint8_t index, uint16_t ARR_val);
void BSP_PWM_SetCCR(uint8_t index, uint16_t CCR_val);
void BSP_PWM_Setduty(uint8_t index, float duty);
void BSP_PWM_Start(uint8_t index);
void BSP_PWM_Stop(uint8_t index);

// ***
// CAN
// ***
#define TRANSMIT_ID 0x305   //发送给主机数据报文的id
#define RECEIVE_ID 0x405   //接收的主机控制报文的id

typedef void rx_callback(uint8_t*, uint16_t);
void BSP_CAN_Init();
void BSP_CAN_RegisterCallback(rx_callback* callback, uint16_t id);
void BSP_CAN_Transmit(uint8_t* pdata, uint16_t len, uint16_t id);

// ****
// GPIO
// ****
void BSP_GPIO_out(uint8_t index, uint8_t output);
uint8_t BSP_GPIO_in(uint8_t index);
void BSP_GPIO_Init();

// ***
// ADC
// ***
//typedef void adc_handle(uint16_t*);
void BSP_ADC_Init();

// ***
// IIC
// ***
typedef enum IIC_comu_status_e
{
    IIC_ERR = 0,  //并没有发完或收完要求规模的数据，传输被接收方叫停
    IIC_OK
}IIC_comu_status;
void BSP_IIC_Init(void);
void BSP_IIC_setpara(uint8_t port_index, GPIO_TypeDef* IOx_CL, uint16_t Pin_CL, GPIO_TypeDef* IOx_DA, uint16_t Pin_DA, uint8_t ADDR);
IIC_comu_status BSP_IIC_sta(uint8_t port_index);
void Master_Transmit(uint8_t port_index, uint8_t* pdata, uint16_t len);
void Master_Receive(uint8_t port_index, uint8_t* rxbuf, uint16_t len);
void Master_Complex(uint8_t port_index, uint8_t* pdata, uint16_t len_t, uint8_t* rxbuf, uint16_t len_r);
void Master_WriteReg(uint8_t port_index, uint8_t adr_reg, uint8_t len, uint8_t* pdata);

#endif