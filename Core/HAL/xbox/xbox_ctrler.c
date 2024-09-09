#include "bsp.h"
#include "string.h"
#include "watch_dog.h"
#include "xbox_ctrler.h"

xbox_ctrler_s xbox_ctrler = {
    .online_flag = 0,
    .commu_on_flag = 0,
}; 

void xbox_lost()
{
    xbox_ctrler.commu_on_flag = 0;
}

void xbox_recv_handle(uint8_t* pdata, uint16_t len)
{
    if (len != 19) return;
    //简单校验
    if (pdata[0] == 33 && pdata[18] == 66) {
        WatchDog_feed(xbox_ctrler.p_Wdog); //收到合法数据，喂狗
        xbox_ctrler.commu_on_flag = 1;
        xbox_ctrler.online_flag =  pdata[1]; //第二个byte表示是否连接到手柄
        if (xbox_ctrler.online_flag) {
            memcpy(&xbox_ctrler.xbox_msg, pdata + 2, 16);
        }
    }
}

void xbox_init()
{
    BSP_UART_registerfunc(xbox_recv_handle, XBOX_UART_RORT);
    WatchDog_Create(50, xbox_lost);
}