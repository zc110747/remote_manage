
#pragma once

#include "main.h"
#include "driver.hpp"

class led_driver
{
public:
    void init(void);
    bool test(void);
    void set(led_device dev, led_status status);
    static led_driver* get_instance(){
        static led_driver instance_;
        return &instance_;
    }

private:
    void hardware_init(void);
};

#define LED0_ON     { led_driver::get_instance()->set(LED0, LED_STATUS_ON);}
#define LED0_OFF    { led_driver::get_instance()->set(LED0, LED_STATUS_OFF);}
#define LED1_ON     { led_driver::get_instance()->set(LED1, LED_STATUS_ON);}
#define LED1_OFF    { led_driver::get_instance()->set(LED1, LED_STATUS_OFF);}