#include "stdint.h"
#include "cvector.h"
#include "watch_dog.h"

cvector* WatchDog_instance;

void WatchDog_Init()
{
    WatchDog_instance = cvector_create(sizeof(WatchDog_s*));
}

WatchDog_s* WatchDog_Create(uint16_t timeout, watchD_callback f_callback)
{
    WatchDog_s* obj = malloc(sizeof(WatchDog_s));
    obj->timeout = timeout;
    obj->tick = 0;
    obj->f_callback = f_callback;
    cvector_pushback(WatchDog_instance, &obj);
}

void WatchDog_feed(WatchDog_s* obj)
{
    obj->tick = 0;
}

void WatchDog_Update()
{
    for (uint8_t i = 0; i < cvector_length(WatchDog_instance); i++){
        WatchDog_s* obj = *(WatchDog_s**)cvector_val_at(WatchDog_instance, i);
        obj->tick++;
        if (obj->tick > obj->timeout) {
            if (obj->f_callback != NULL) {
                obj->f_callback;
            }
        }
    }
}