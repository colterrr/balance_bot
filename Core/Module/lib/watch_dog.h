#ifndef _WATCH_DOG_H_
#define _WATCH_DOG_H_

#include "stdint.h"

typedef void (*watchD_callback)(void*);

typedef struct
{
    watchD_callback f_callback;
    uint16_t tick;
    uint16_t timeout; //规定超时的计数量，线程周期为10ms
}WatchDog_s;

void WatchDog_Init();
void WatchDog_feed(WatchDog_s* obj);
WatchDog_s* WatchDog_Create(uint16_t timeout, watchD_callback f_callback);
void WatchDog_Update();

#endif