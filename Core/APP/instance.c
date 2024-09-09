#include "bsp.h"
#include "myfunc.h"
#include "encoder.h"
#include "imu.h"
#include "foc.h"
#include "xbox_ctrler.h"

#define DEBUG 1
#if !DEBUG
#include "BLDC_motor_can.h"
#endif
#if DEBUG
#include "BLDC_motor.h"
#endif

typedef enum run_mode_e
{
    ROBOT_STOP = 0,
    ROBOT_RUN,
}run_mode;
typedef struct instance_s
{
    #if !DEBUG
    run_mode run_mode;

    imu_atk* imu;
    BLDC_motor_can* motor_lf;
    BLDC_motor_can* motor_lb;
    BLDC_motor_can* motor_l_wheel;
    BLDC_motor_can* motor_rf;
    BLDC_motor_can* motor_rb;
    BLDC_motor_can* motor_r_wheel;    
    #endif 

    #if DEBUG
    // 调试用
    BLDC_motor* motor1;
    float theta;
    uint8_t zone;
    float a;
    float b;
    #endif
}instance;

instance robot;
extern xbox_ctrler_s xbox_ctrler;

void Instance_init()
{
    #if DEBUG   
    foc_config foc_cfg;
    foc_cfg.foc_type = VOLTAGE_OPEN;
    foc_cfg.index_a = 2;
    foc_cfg.index_b = 0;
    foc_cfg.index_c = 1;

    // pid_config pos_cfg;
    // pos_cfg.Kp = 1;

    BLDC_motor_config motor_config;
    motor_config.mode = TORQUE_MODE;    
    motor_config.en_port = MOTOR1_PORT;
    motor_config.foc_cfg = foc_cfg;
    motor_config.pole_pair = 7;
    motor_config.voltage_max = 12;
    motor_config.torque_cons = 0.06;
    robot.motor1 = BLDC_motor_Create(motor_config);
    robot.motor1->motor_encoder = Encoder_Create(1, GPIOD, GPIO_PIN_4, GPIOD, GPIO_PIN_3);
    #endif

    #if !DEBUG
    //robot.imu = imu_Create();
    BLDC_motor_can_config config_tem;
    //左侧电机
    config_tem.id = 0x300;
    robot.motor_l_wheel = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x301;
    robot.motor_lf = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x302;
    robot.motor_lb = BLDC_MotorCan_Create(config_tem);
    //右侧电机
    config_tem.id = 0x303;
    robot.motor_r_wheel = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x304;
    robot.motor_rf = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x305;
    robot.motor_rb = BLDC_MotorCan_Create(config_tem);
    #endif

}

void Instance_Update()
{
    //if (robot.motor1->init_flag == 0)return;
    // if (robot.motor1->EN == 1 ){
    //     BSP_GPIO_out(MOTOR1_PORT, 1);
    //     vector_ab ab;
    //     for (uint8_t i = 0; i < 28; i++){
    //         robot.theta = 3.14159/14*i;
    //         ab.alpha = 0.5*cos(3.14159/14*i);
    //         robot.a = ab.alpha;
    //         ab.beta = 0.5*sin(3.141592/14*i);
    //         robot.b = ab.beta;
    //         robot.zone = SVPWM(robot.motor1->motor_foc, ab);
    //         delay_ms_sss(1000);            
    //     }

    // }
    // else BSP_GPIO_out(MOTOR1_PORT, 0);
}