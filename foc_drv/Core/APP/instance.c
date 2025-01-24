#include "instance.h"
#include "bsp.h"
#include "BLDC_motor.h"
#include "myfunc.h"

typedef struct focdrv_s
{
    BLDC_motor* motor;
}focdrv;

focdrv ins;
static uint8_t motor_txbuf[8]; //标准CAN最大数据长8bytes
#define WATCH_TIME 30 //掉线超时次数
static uint8_t watchDog;

#define MOTOR_TYPE_7

void master_info_handle(uint8_t* pdata, uint16_t len)
{
    if (len != 5)return;
    /*
    接受主机的控制报文：
    模式 1byte 0停止 1力矩 2速度 3位置
    目标值 4byte (float_32t)
    */
    if (pdata[0] == 0)ins.motor->EN = 0;
    else {
        ins.motor->EN = 1;
        ins.motor->config.mode = pdata[0];
    } 
   ins.motor->ref = *(float*)(pdata + 1);
}

void Instance_Init()
{
    foc_config foc_cfg;
    foc_cfg.foc_type = VOLTAGE_OPEN;
    foc_cfg.index_a = 1;
    foc_cfg.index_b = 0;
    foc_cfg.index_c = 2;

    pid_config pos_cfg;
    pid_config spd_cfg;

    BLDC_motor_config motor_config;
    motor_config.foc_cfg = foc_cfg;
    motor_config.config_pos = pos_cfg;
    motor_config.config_spd = spd_cfg;

    #if defined MOTOR_TYPE_11
    motor_config.mode = TORQUE_MODE; //完全功能是这里模式可选
    motor_config.en_port = 1;
    motor_config.fault_port = 2;
    motor_config.pole_pair = 11; 
    motor_config.torque_cons = 0.2;
    motor_config.voltage_max = 12; 
    motor_config.R_phase = 9;   
    ins.motor = BLDC_motor_Create(motor_config);
    ins.motor->motor_encoder = Encoder_Create(0, GPIOB, GPIO_PIN_3, GPIOA, GPIO_PIN_15);
    #endif

    #if defined MOTOR_TYPE_7
    motor_config.mode = TORQUE_MODE; //完全功能是这里模式可选
    motor_config.en_port = 1;
    motor_config.fault_port = 2;
    motor_config.pole_pair = 7; 
    motor_config.torque_cons = 0.06;
    motor_config.voltage_max = 12;
    motor_config.R_phase = 7.1;  
    ins.motor = BLDC_motor_Create(motor_config);
    ins.motor->motor_encoder = Encoder_Create(0, GPIOB, GPIO_PIN_3, GPIOA, GPIO_PIN_15);
    #endif
    BSP_CAN_RegisterCallback(master_info_handle, RECEIVE_ID);
    watchDog = 0;
}

void Instance_Update()
{
    /*
    发送主机的数据报文：
    初始位置 
    位置 4byte   (float32_t)
    角速度 4byte (float32_t)
    */
   *(float*)motor_txbuf = ins.motor->pos;
   *(float*)(motor_txbuf + 4) = ins.motor->w;
    BSP_CAN_Transmit(motor_txbuf, 8, TRANSMIT_ID);
    watchDog ++;
    if (watchDog > 30) {
        ins.motor->commu_sta = COMMU_ERR;
    }
    else {
        ins.motor->commu_sta = COMMU_ON;
    }
}
