
#ifndef _LED_H
#define _LED_H

#include "interface.h"

#define LED0_ON     { led_set(LED0, LED_STATUS_ON);}
#define LED0_OFF    { led_set(LED0, LED_STATUS_OFF);}
#define LED1_ON     { led_set(LED1, LED_STATUS_ON);}
#define LED1_OFF    { led_set(LED1, LED_STATUS_OFF);}

#ifdef __cplusplus
    extern "C"
    {
#endif
        
BaseType_t led_init(void);
void led_set(led_device dev, led_status status);
        
#ifdef __cplusplus
    }
#endif

#endif