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

// *****
// FDCAN
// *****
typedef void can_rx_func(uint16_t id, uint8_t* pdata, uint16_t len);
void BSP_FDCAN_Init();
void BSP_FDCAN_RegisterCallback(uint8_t can_module_index, can_rx_func* callback);
void BSP_FDCAN_Transmit(uint8_t* pdata, uint16_t len, uint16_t id);

// ****
// GPIO
// ****
#define ENCODER_DIR_PORT 0
#define MOTOR1_PORT 1
void BSP_GPIO_out(uint8_t index, uint8_t output);
uint8_t BSP_GPIO_in(uint8_t index);
void BSP_GPIO_Init();

// ***
// IIC
// ***
//software_IIC_Port结构体下表示传输状态的枚举变量
typedef enum
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

// ****
// UART
// ****
typedef void (uart_rx_func)(UART_HandleTypeDef* huart, uint8_t* rxbuffer, uint16_t len);
void BSP_UART_Init(void);
void BSP_UART_send(uint8_t port_index, uint8_t* pdata, uint16_t len);
void BSP_UART_IRQHandler(UART_HandleTypeDef* huart);
void BSP_UART_registerfunc(uart_rx_func* user_func, uint8_t port_index);

// ***
// SPI
// ***
void BSP_SPI_Transmit_Byte(uint8_t port_index, uint8_t* pdata);
void BSP_SPI_Init(void);
void BSP_SPI_Receive(uint8_t port_index, uint8_t* rxbuf, uint8_t len);