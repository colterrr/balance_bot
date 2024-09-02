#include "stdlib.h"
#include "string.h"
#include "myfunc.h"
#include "stdint.h"
#include "main.h"

// 默认设置：
// 从机地址7bit
// 时钟周期8us
// 时序参数如下
// t_HIGH = t_LOW = 4us
// t_HD-DAT = t_SU-DAT = t_HD-STA = t_SU-STA = t_HD-STO = t_SU-STO = 2us
// t_R_SDA, t_F_SDA, t_R_SCL, t_F_SCL 忽略不计

#define H_SCL HAL_GPIO_WritePin(GPIOx_CL,GPIO_Pin_CL,GPIO_PIN_SET)
#define L_SCL HAL_GPIO_WritePin(GPIOx_CL,GPIO_Pin_CL,GPIO_PIN_RESET)

#define H_SDA HAL_GPIO_WritePin(GPIOx_DA,GPIO_Pin_DA,GPIO_PIN_SET)
#define L_SDA HAL_GPIO_WritePin(GPIOx_DA,GPIO_Pin_DA,GPIO_PIN_RESET)

#define READ_DA HAL_GPIO_ReadPin(GPIOx_DA,GPIO_Pin_DA)

//software_IIC_Port结构体下表示传输状态的枚举变量
typedef enum
{
    IIC_ERR = 0,  //并没有发完或收完要求规模的数据，传输被接收方叫停
    IIC_OK
}IIC_comu_status;

typedef enum
{
    ACK = 0,
    NACK
}ACK_value;

typedef struct
{
    //uint16_t speed_kHZ;
    uint8_t slave_ADDR;        //7位从机地址，最高位无意义
    IIC_comu_status status;

    GPIO_TypeDef* IOx_CL;
    GPIO_TypeDef* IOx_DA;
    uint16_t Pin_CL;
    uint16_t Pin_DA;
}software_IIC_Port;

static GPIO_TypeDef* GPIOx_CL;
static GPIO_TypeDef* GPIOx_DA;
static uint16_t GPIO_Pin_CL;
static uint16_t GPIO_Pin_DA;

#define IIC_MAX_NUM 2
software_IIC_Port IIC_port[IIC_MAX_NUM];

/***SDA输出输入模式改变***/

static void SDA_Set_Output()
{
    GPIO_InitTypeDef config ={0};
    config.Mode = GPIO_MODE_OUTPUT_PP;
    config.Pin = GPIO_Pin_DA;
    config.Pull = GPIO_NOPULL;
    config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_DeInit(GPIOx_DA, GPIO_Pin_DA);
    HAL_GPIO_Init(GPIOx_DA, &config);
}

static void SDA_Set_Input()
{
    GPIO_InitTypeDef config = {0};
    config.Mode = GPIO_MODE_INPUT;
    config.Pin = GPIO_Pin_DA;
    config.Pull = GPIO_NOPULL;
    config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_DeInit(GPIOx_DA, GPIO_Pin_DA);
    HAL_GPIO_Init(GPIOx_DA, &config);
}

/***-------------------***/


/***开始信号S和结束信号P***/

static void Master_Start()
{
    SDA_Set_Output();
    H_SCL;
    H_SDA;
    delay_us(2);//t_SU-STA 2
    L_SDA;
    delay_us(2);//t_HD-STA 2
    L_SCL; //拉低SCL，利于SDA电平变化
}

static void Master_Stop()
{
    SDA_Set_Output();
    L_SCL;
    L_SDA;
    delay_us(4);//t_LOW 4
    H_SCL;
    delay_us(2);//t_SU-STO 2
    H_SDA;
}

/***---------------------***/


/***接收应答和发送应答***/

static void Master_N_ACK(ACK_value sig)
{
    SDA_Set_Output();
    L_SCL;
    delay_us(2);
    if(sig)
        H_SDA;
    else 
        L_SDA;
    delay_us(2);
    H_SCL;
    delay_us(4);
    L_SCL;
}

static uint8_t Master_wait_ACK()
{
    L_SCL;
    SDA_Set_Input();
    uint16_t outtime = 10000;
    delay_us(4);
    H_SCL;
    delay_us(4);
    while(READ_DA)
    {
        if(--outtime)
        {
            Master_Stop();
            return 0xff; //表示err
        }
    }
    L_SCL;
    return 0;
}

/***------------------***/


/***发送和接收一个字节数据***/

static void Master_Transmit_Byte(uint8_t Byte)
{
    SDA_Set_Output();
    uint8_t i = 8;
    L_SCL;
    //发最高位，每次左移一位替换最高位
    while(i-- > 0){
        delay_us(2);//t_HD-DAT
        if (Byte & 0x80) H_SDA;
        else L_SDA;
        Byte<<=1;
        delay_us(2);//t_SU-DAT
        H_SCL;
        delay_us(2);//t_HIGH
        L_SCL;
    }
}

static uint8_t Master_Receive_Byte()
{
    SDA_Set_Input();
    uint8_t i = 0;
    uint8_t Byte = 0;
    while(i++ < 8){
        Byte<<=1;    
        L_SCL;
        delay_us(2);//t_LOW
        H_SCL;
        if (READ_DA) Byte++;  
        delay_us(2);//t_HIGH
    }
    L_SCL;
    return Byte;
}

/***-------------------------***/



/***发送和接收多字节数据***/

void Master_Transmit(uint8_t port_index, uint8_t* pdata, uint16_t len)
{
    software_IIC_Port* port = IIC_port + port_index;
    
    GPIOx_CL = port->IOx_CL;
    GPIO_Pin_CL = port->Pin_CL;
    GPIOx_DA = port->IOx_DA;
    GPIO_Pin_DA = port->Pin_DA;
    uint8_t ADDR = port->slave_ADDR;

    uint8_t i = 0;

    Master_Start();
    Master_Transmit_Byte(ADDR << 1);//从机地址 + W
    if(Master_wait_ACK() == 0xff){
        //收到NACK
        Master_Stop();
        port->status = IIC_ERR;
        return;
    }

    for(i = 0; i < len; i++){
        Master_Transmit_Byte(pdata[i]);
        if(Master_wait_ACK() == 0xff){
            //收到NACK
            Master_Stop();
            port->status = IIC_ERR;
            return;
        }
    }

    Master_Stop();
    port->status = IIC_OK;
    return;
}

void Master_Receive(uint8_t port_index, uint8_t* rxbuf, uint16_t len)
{
    software_IIC_Port* port = IIC_port + port_index;

    GPIOx_CL = port->IOx_CL;
    GPIO_Pin_CL = port->Pin_CL;
    GPIOx_DA = port->IOx_DA;
    GPIO_Pin_DA = port->Pin_DA;
    uint8_t ADDR = port->slave_ADDR;

    uint8_t i = 0;

    Master_Start();
    Master_Transmit_Byte((ADDR << 1 )| 1);//从机地址 + R
    if (Master_wait_ACK() == 0xff) {
        //收到从机的NACK，停止接收
        Master_Stop();
        port->status = IIC_ERR;
        return;
    } 
    for(i = 0; i < len; i++){
        rxbuf[i] = Master_Receive_Byte();
        Master_N_ACK(!(len - i - 1)); //根据是否收完足够的数据来发送ACK或NACK
    }

    Master_Stop();
    port->status = IIC_OK;
    return;
}

void Master_Complex(uint8_t port_index, uint8_t* pdata, uint16_t len_t, uint8_t* rxbuf, uint16_t len_r)
{
    software_IIC_Port* port = IIC_port + port_index;

    GPIOx_CL = port->IOx_CL;
    GPIO_Pin_CL = port->Pin_CL;
    GPIOx_DA = port->IOx_DA;
    GPIO_Pin_DA = port->Pin_DA;
    uint8_t ADDR = port->slave_ADDR;

    uint8_t i = 0;

    Master_Start();
    Master_Transmit_Byte(ADDR << 1);//从机地址 + W
    if(Master_wait_ACK() == 0xff){
        //收到从机的NACK，停止发送
        Master_Stop();
        port->status = IIC_ERR;
        return;
    }
   for(i = 0; i < len_t; i++){
        Master_Transmit_Byte(pdata[i]);
        if(Master_wait_ACK() == 0xff){
            //收到从机的NACK，停止发送
            Master_Stop();
            port->status = IIC_ERR;
            return;
        }
    }

    Master_Start();//复合格式二次起始信号
    //delay_ms(1);   //?????为什么要等一会?????
    Master_Transmit_Byte((ADDR << 1 )| 1);//从机地址 + R
    if (Master_wait_ACK() == 0xff) {
        //收到从机的NACK，停止接收
        Master_Stop();
        port->status = IIC_ERR;
        return;
    } 
    for(i = 0; i < len_r; i++){
        rxbuf[i] = Master_Receive_Byte();
        Master_N_ACK(!(len_r - i - 1)); //根据是否收完足够的数据来发送ACK或NACK
    }

    Master_Stop();
    port->status = IIC_OK;
    return;
}

/***-----------------------------------------------***/




/***----查询寄存器地址数据的通讯方式----***/

void Master_ReadReg_Byte(uint8_t port_index, uint8_t adr_reg, uint8_t* rxbuf)
{
    Master_Complex(port_index, &adr_reg, 1, rxbuf, 1);
}

void Master_ReadReg_16bit(uint8_t port_index, uint8_t adr_reg, uint16_t* pdata)
{
    uint8_t rxbuf[2];
    Master_ReadReg_Byte(port_index, adr_reg, rxbuf);
    Master_ReadReg_Byte(port_index, adr_reg, rxbuf + 1);
    *pdata = (uint16_t)rxbuf[1] << 8 | rxbuf[0];
}

void Master_WriteReg(uint8_t port_index, uint8_t adr_reg, uint8_t len, uint8_t* pdata)
{
    software_IIC_Port* port = IIC_port + port_index;

    GPIOx_CL = port->IOx_CL;
    GPIO_Pin_CL = port->Pin_CL;
    GPIOx_DA = port->IOx_DA;
    GPIO_Pin_DA = port->Pin_DA;
    uint8_t ADDR = port->slave_ADDR;

    uint8_t i = 0;
    Master_Start();
    Master_Transmit_Byte(ADDR << 1);//从机地址 + W
    if(Master_wait_ACK() == 0xff){
        //收到从机的NACK，停止发送
        Master_Stop();
        port->status = IIC_ERR;
        return;
    }
    Master_Transmit_Byte(adr_reg);
    if(Master_wait_ACK() == 0xff){
        //收到从机的NACK，停止发送
        Master_Stop();
        port->status = IIC_ERR;
        return;
    }
   for(i = 0; i < len; i++){
        Master_Transmit_Byte(pdata[i]);
        if(Master_wait_ACK() == 0xff){
            //收到从机的NACK，停止发送
            Master_Stop();
            port->status = IIC_ERR;
            return;
        }
    }
    Master_Stop();
    port->status = IIC_OK;
    return;
}

/***-----------------------------------------------***/

void BSP_IIC_Init(void)
{

}

void BSP_IIC_setpara(uint8_t port_index, GPIO_TypeDef* IOx_CL, uint16_t Pin_CL, GPIO_TypeDef* IOx_DA, uint16_t Pin_DA, uint8_t ADDR)
{
    IIC_port[port_index].IOx_CL = IOx_CL;
    IIC_port[port_index].IOx_DA = IOx_DA;
    IIC_port[port_index].Pin_CL = Pin_CL;
    IIC_port[port_index].Pin_DA = Pin_DA;
    IIC_port[port_index].slave_ADDR = ADDR;
}

IIC_comu_status BSP_IIC_sta(uint8_t port_index)
{
    return IIC_port[port_index].status;
}