

#pragma once

#include "includes.hpp"
#include "key.hpp"

class motion_manage
{
public:  
    bool init();
    
    static motion_manage* get_instance()
    {
        static motion_manage instance_;
        return &instance_;
    }

    KEY_STATE anti_shake(uint8_t *pTick, KEY_STATE nowIoStatus, KEY_STATE readIoStatus);
    bool is_time_escape(uint32_t ticks, uint32_t time);
private:
    static void run(void* parameter);
    
private:
    TaskHandle_t task_handle_{nullptr};
};