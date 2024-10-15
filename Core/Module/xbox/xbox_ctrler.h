#ifndef _XBOX_CTRLER_H_
#define _XBOX_CTRLER_H_

#include "stdint.h"
#include "watch_dog.h"

typedef struct
{
    uint16_t left_x;       ///< 左0~右65535
    uint16_t left_y;       ///< 上0~下65535
    uint16_t right_x;      ///< 左0~右65535
    uint16_t right_y;      ///< 上0~下65535
    uint16_t triger_left;  ///< 不按0~按到底1023
    uint16_t triger_right; ///< 不按0~按到底1023
    uint8_t key_dir;       ///< 1~8 垂直向上为1，顺时针旋转递增，不按为0
    struct
    {
        uint8_t key_a : 1;
        uint8_t key_b : 1;
        uint8_t  : 1;
        uint8_t key_x : 1;
        uint8_t key_y : 1;
        uint8_t : 1;
        uint8_t key_lb : 1;
        uint8_t key_rb : 1;
    };
    struct
    {
        uint8_t : 2;
        uint8_t key_page: 1;
        uint8_t key_menu : 1;
        uint8_t : 4;
    };
    uint8_t share;
} xbox_message;

typedef struct
{
    uint8_t commu_on_flag; //与esp32通讯是否正常
    uint8_t online_flag;   //手柄是否连接
    xbox_message xbox_msg;
    WatchDog_s* p_Wdog;
}xbox_ctrler_s;
#pragma pack()

void xbox_init();

#endif