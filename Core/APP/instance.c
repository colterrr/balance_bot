#include "instance.h"

#include "bsp_gpio.h"
#include "myfunc.h"

instance robot;

void Instance_init()
{
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



    //robot.imu = imu_Create();
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