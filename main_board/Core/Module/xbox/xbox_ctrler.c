#include "bsp.h"
#include "string.h"
#include "watch_dog.h"
#include "xbox_ctrler.h"

xbox_ctrler_s my_xbox = {};

static void xbox_lost()
{
    my_xbox.commu_on_flag = 0;
}

void xbox_recv_handle(uint8_t* pdata, uint16_t len)
{
    if (len != 19) return;
    //简单校验
    if (pdata[0] == 33 && pdata[18] == 66) {
        WatchDog_feed(my_xbox.p_Wdog); //收到合法数据，喂狗
        my_xbox.commu_on_flag = 1;
        my_xbox.online_flag =  pdata[1]; //第二个byte表示是否连接到手柄
        if (my_xbox.online_flag) {
            my_xbox.xbox_last_msg.last_a = my_xbox.xbox_msg.key_a;
            my_xbox.xbox_last_msg.last_b = my_xbox.xbox_msg.key_b;
            memcpy(&my_xbox.xbox_msg, pdata + 2, 16);
            if (up_A) {
                my_xbox.xbox_ctrl_msg.manual_stop = !my_xbox.xbox_ctrl_msg.manual_stop; //A键按下时切换启停
            }
        }
    }
}

void xbox_init()
{
    BSP_UART_registerfunc(xbox_recv_handle, XBOX_UART_RORT);
    my_xbox.p_Wdog = WatchDog_Create(50, xbox_lost);
}