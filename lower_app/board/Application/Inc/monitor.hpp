#pragma once

#include "driver.hpp"

#define KEY_NUM     5

class monitor_manage
{
public: 
    bool init();
    
    static monitor_manage* get_instance()
    {
        static monitor_manage instance_;
        return &instance_;
    }

    KEY_STATE anti_shake(uint8_t *pTick, KEY_STATE nowIoStatus, KEY_STATE readIoStatus);
    bool is_time_escape(uint32_t ticks, uint32_t time);
private:
    static void run(void* parameter);
    static void key_motion();
    static void timer_loop_motion();
    static void adc_monitor();

private:
    TaskHandle_t task_handle_{nullptr};
    static KEY_STATE key_last_[KEY_NUM];
    static KEY_STATE key_now_[KEY_NUM];
    static uint8_t tick[KEY_NUM];
};