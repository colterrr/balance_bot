#include "imu.h"
#include "cvector.h"
#include "bsp_iic.h"
#include "myfunc.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
cvector* my_imu;

void imu_data_get(imu_atk* obj)
{
    uint8_t raw_gyro[6];uint16_t gyro[3];
    uint8_t raw_acc[6];uint16_t acc[3];
    uint8_t reg = 0;
    //温度数据
    long aa;
    mpu_get_temperature(&aa, NULL);
    obj->temperature = aa / 65536.0; //q0转q16
    //角速度数据
    reg = READ_GYRO_REG;
    Master_Complex(IMU_IIC_PORT, &reg, 1, raw_gyro, 6);
    gyro[0] = ((uint16_t)raw_gyro[0] << 8) | raw_gyro[1];
    gyro[1] = ((uint16_t)raw_gyro[2] << 8) | raw_gyro[3];
    gyro[2] = ((uint16_t)raw_gyro[4] << 8) | raw_gyro[5];
    for (uint8_t i = 0; i < 3; i++){
        if (gyro[i] & 0x8000){
            gyro[i]--;
            gyro[i] = ~gyro[i];
            obj->gyro[i] = (gyro[i] & ~0x8000);
            obj->gyro[i] *= GYRO_FSR / 32768.0;
            obj->gyro[i] *= -1;
        }
        else {
        obj->gyro[i] = (gyro[i] & ~0x8000);
        obj->gyro[i] *= GYRO_FSR / 32768.0;
        }
    }
    //加速度数据
    reg = READ_ACCE_REG;
    Master_Complex(IMU_IIC_PORT, &reg, 1, raw_acc,  6);
    acc[0] = ((uint16_t)raw_acc[0] << 8) | raw_acc[1];
    acc[1] = ((uint16_t)raw_acc[2] << 8) | raw_acc[3];
    acc[2] = ((uint16_t)raw_acc[4] << 8) | raw_acc[5];
    for (uint8_t i = 0; i < 3; i++){
        if (acc[i] & 0x8000){
            acc[i]--;
            acc[i] = ~acc[i];
            obj->acc[i] = (acc[i] & ~0x8000);
            obj->acc[i] *= GRAVITY * ACCE_FSR / 32768.0;
            obj->acc[i] *= -1;
        }
        else {
        obj->acc[i] = (acc[i] & ~0x8000);
        obj->acc[i] *= GRAVITY * ACCE_FSR / 32768.0;
        }
    }
    //欧拉角
    atk_ms6050_dmp_get_data(&obj->pitch, &obj->roll, &obj->yaw);
}

void IMU_Update(void)
{
    for(uint8_t i = 0; i < my_imu->cv_len; i++){
        imu_atk* obj = *(imu_atk**)cvector_val_at(my_imu, i);
        imu_data_get(obj);
        float pitch = atan2((float)(0 - obj->acc[0]), obj->acc[2]);
        obj->mahony_handler.q0 = cos(pitch / 2);
        obj->mahony_handler.q1 = 0;
        obj->mahony_handler.q2 = sin(pitch / 2);
        obj->mahony_handler.q3 = 0;
        MahonyAHRS_update(&obj->mahony_handler, obj->gyro[0], obj->gyro[1], obj->gyro[2], obj->acc[0], obj->acc[1], obj->acc[2]);
        obj->eule_deg[0] = obj->mahony_handler.euler[0] * 180 / 3.14159;
        obj->eule_deg[1] = obj->mahony_handler.euler[1] * 180 / 3.14159;
        obj->eule_deg[2] = obj->mahony_handler.euler[2] * 180 / 3.14159;
    }
}

void IMU_Init()
{
    my_imu = cvector_create(sizeof(imu_atk*));
    BSP_IIC_setpara(IMU_IIC_PORT, IMU_IOx_CL, IMU_Pin_CL, IMU_IOx_DA, IMU_Pin_DA, IMU_ADDR_GND);
    // Master_WriteReg_Byte(IMU_IIC_PORT, PWR_CTRL_REG_1, 0x80);
    // delay_ms(100);
    // Master_WriteReg_Byte(IMU_IIC_PORT, PWR_CTRL_REG_1, 0x00);
    // delay_ms(100);
    // Master_WriteReg_Byte(IMU_IIC_PORT, gyro_RANGE_REG, 3<<3);

    // Master_WriteReg_Byte(IMU_IIC_PORT, FIFO_EN_REG, 0x00);
    // Master_WriteReg_Byte(IMU_IIC_PORT, SAMPLE_PREC_REG, 0x13);//采样频率 1k / (1 + 19) = 50Hz
    // Master_WriteReg_Byte(IMU_IIC_PORT, SENSOR_CONFIG_REG, 0x04);
    // Master_WriteReg_Byte(IMU_IIC_PORT, PWR_CTRL_REG_2, 0x00);
    // Master_WriteReg_Byte(IMU_IIC_PORT, PWR_CTRL_REG_1, 0x01);
    uint8_t sta = atk_ms6050_dmp_init();
    mpu_set_accel_fsr(ACCE_FSR);
    mpu_set_gyro_fsr(GYRO_FSR);
}

imu_atk* imu_Create(void)
{
    imu_atk* pt = (imu_atk*)malloc(sizeof(imu_atk));
    memset(pt, 0, sizeof(imu_atk));
    cvector_pushback(my_imu, &pt);
    MahonyAHRS_init(&pt->mahony_handler, 1, 0.08, 100);
    return pt;
}

int atk_ms6050_write(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* pdata)
{
    Master_WriteReg(IMU_IIC_PORT, reg, len, pdata);
    if (BSP_IIC_sta(IMU_IIC_PORT) == IIC_OK)return 0;
    else return 1;
}

int atk_ms6050_read(uint8_t addr, uint8_t reg, uint8_t len, uint8_t* rxbuf)
{
    Master_Complex(IMU_IIC_PORT, &reg, 1, rxbuf, len);
    if (BSP_IIC_sta(IMU_IIC_PORT) == IIC_OK)return 0;
    else return 1;
}