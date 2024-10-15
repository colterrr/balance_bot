#include "bsp.h"
#include "myfunc.h"
#include "encoder.h"
#include "imu.h"
#include "foc.h"
#include "xbox_ctrler.h"
#include "BLDC_motor_can.h"
#include "lqrrr.h"
#include "pos_calc.h"
#include "spd_calc.h"
#include "out_calc.h"

#define DEBUG 0

#if DEBUG
#include "BLDC_motor.h"
#endif

typedef enum
{
    ROBOT_STOP = 0,
    ROBOT_RUN,
}run_mode_e;

typedef struct instance_s
{
    #if !DEBUG
    run_mode_e run_mode;

    imu_atk* imu;
    BLDC_motor_can* motor_lf;
    BLDC_motor_can* motor_lb;
    BLDC_motor_can* motor_l_wheel;
    BLDC_motor_can* motor_rf;
    BLDC_motor_can* motor_rb;
    BLDC_motor_can* motor_r_wheel;    

    double statesl[6];
    double pos_l[2]; //fai0 L0
    double spd_l[2]; //w0  vL0
    double statesr[6];
    double pos_r[2]; //fai0 L0
    double spd_r[2]; //w0  vL0 
    double ref_x;
    double ref_v;
    double ref_yaw;
    #endif 

    #if DEBUG
    // 调试用
    BLDC_motor* test_motor;
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

    pid_config spd_cfg;
    spd_cfg.Kp = 0.01;
    spd_cfg.Ki = 0;
    spd_cfg.Kd = 0;
    spd_cfg.fdb_p = &robot.test_motor->w;
    spd_cfg.err_max = 5;
    spd_cfg.output_max = 1;
    spd_cfg.reverse_flag = 0;

    BLDC_motor_config motor_config;
    motor_config.mode = SPEED_MODE;    
    motor_config.en_port = MOTOR1_PORT;
    motor_config.foc_cfg = foc_cfg;
    motor_config.config_spd = spd_cfg;
    motor_config.pole_pair = 7;
    motor_config.voltage_max = 12;
    motor_config.torque_cons = 0.06;
    motor_config.R_phase = 7.8;
    robot.test_motor = BLDC_motor_Create(motor_config);
    robot.test_motor->motor_encoder = Encoder_Create(1, GPIOD, GPIO_PIN_4, GPIOD, GPIO_PIN_3);
    #endif

    #if !DEBUG
    robot.imu = imu_Create();

    BLDC_motor_can_config config_tem;
    //左侧电机
    config_tem.id = 0x400;
    robot.motor_l_wheel = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x401;
    robot.motor_lf = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x402;
    robot.motor_lb = BLDC_MotorCan_Create(config_tem);
    //右侧电机
    config_tem.id = 0x403;
    robot.motor_r_wheel = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x404;
    robot.motor_rf = BLDC_MotorCan_Create(config_tem);
    config_tem.id = 0x405;
    robot.motor_rb = BLDC_MotorCan_Create(config_tem);
    #endif

}

void Instance_Update()
{
    #if DEBUG
    if (xbox_ctrler.commu_on_flag && xbox_ctrler.online_flag) ;
    else return;

    if (robot.test_motor->init_flag == 0)return;
    if (robot.test_motor->EN == 1 ){
        BSP_GPIO_out(MOTOR1_PORT, 1);
        if (xbox_ctrler.xbox_msg.right_y > 31000 && xbox_ctrler.xbox_msg.right_y < 35000)
            robot.test_motor->ref = 0;
        else robot.test_motor->ref = (float)(xbox_ctrler.xbox_msg.right_y - 32768) / 32768 * 100;

        // vector_ab ab;
        // for (uint8_t i = 0; i < 28; i++){
        //     robot.theta = 3.14159/14*i;
        //     ab.alpha = 0.5*cos(3.14159/14*i);
        //     robot.a = ab.alpha;
        //     ab.beta = 0.5*sin(3.141592/14*i);
        //     robot.b = ab.beta;
        //     robot.zone = SVPWM(robot.test_motor->motor_foc, ab);
        //     delay_ms_sss(1000);            
        // }

    }
    else BSP_GPIO_out(MOTOR1_PORT, 0);

    #endif

    #if !DEBUG
    //各通讯监测
    // if (xbox_ctrler.commu_on_flag && xbox_ctrler.online_flag) ;
    // else robot.run_mode = ROBOT_STOP;

    // if (robot.run_mode == ROBOT_STOP) {
    //     robot.motor_l_wheel->EN = 0;
    //     robot.motor_lb->EN = 0;
    //     robot.motor_lf->EN = 0;
    //     robot.motor_r_wheel->EN = 0;
    //     robot.motor_rb->EN = 0;
    //     robot.motor_rf->EN = 0;
    //     return;
    // }

    // //期望值
    // if (xbox_ctrler.xbox_msg.right_y > 31000 && xbox_ctrler.xbox_msg.right_y < 35000)
    //     robot.ref_v = 0;
    // else robot.ref_v = (float)(xbox_ctrler.xbox_msg.right_y - 32768) / 32768 * 100;

    // //vmc
    // pos_calc(robot.motor_lb->pos, robot.motor_lf->pos, robot.pos_l);
    // spd_calc(robot.motor_lb->w, robot.motor_lf->pos, robot.motor_lb->w, robot.motor_lf->w,  robot.spd_l);

    // robot.statesl[0] = robot.motor_l_wheel->pos;

    // // lqrrr(robot.statesl, robot.pos_l[1], robot.ref_x, )
    // // out_calc()

    // pos_calc(robot.motor_rb->pos, robot.motor_rf->pos, robot.pos_r);
    // spd_calc(robot.motor_rb->w, robot.motor_rf->pos, robot.motor_rb->w, robot.motor_rf->w,  robot.spd_r);



    #endif
}