#ifndef _IMU_H_
#define _IMU_H_

#include "stdint.h"
#include "gpio.h"
#include "MahonyAHRS.h"

#define IMU_ADDR_GND 0x68
#define IMU_ADDR_VCC 0x69

#define IMU_IIC_PORT 0
#define IMU_IOx_CL GPIOB
#define IMU_IOx_DA GPIOB
#define IMU_Pin_CL GPIO_PIN_6
#define IMU_Pin_DA GPIO_PIN_7

#define PWR_CTRL_REG_1 0x6B
#define PWR_CTRL_REG_2 0x6C
#define FIFO_EN_REG 0x23
#define EULE_RANGE_REG 0x1B
#define ACCE_RANGE_REG 0x1C
#define SAMPLE_PREC_REG 0x19
#define SENSOR_CONFIG_REG 0x1A

#define READ_GYRO_REG 0x43
#define READ_ACCE_REG 0x3B
#define READ_TEMP_REG 0x41

#define ACCE_FSR 2   //加速度full scale range 4g
#define GYRO_FSR 2000 //角速度full scale range 2000°/s
#define GRAVITY 9.78 //当地重力加速度

typedef struct imu_atk_s
{
    float temperature;
    float gyro[3];
    float acc[3];
    float pitch;
    float yaw;
    float roll;
    MahonyAHRS mahony_handler;
    uint8_t init_sta;
    float eule_deg[3];
}imu_atk;

void IMU_Update(void);
void IMU_Init();
imu_atk* imu_Create(void);

//这两句会被inv_mpu调用，别删
int atk_ms6050_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* pdata);
int atk_ms6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* rxbuf);

#endif