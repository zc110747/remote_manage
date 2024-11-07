//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      key.cpp
//
//  Purpose:
//      按键管理模块,处理按键的初始化和注册回调事件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <linux/input.h>
#include "key.hpp"
#include "logger_manage.hpp"
#include "common.hpp"

typedef struct
{
    uint16_t key_num;
    uint16_t key_event;
    std::function<void(uint16_t, uint16_t)> func;
}KEY_ACTION_STRUCT;

static std::vector<KEY_ACTION_STRUCT> key_action_list;

bool key_device::init(const std::string &DevicePath, int flags)
{
    static bool is_first_run = true;
    device_path_ = DevicePath;

    device_fd_ = ::open(device_path_.c_str(), flags);
    if (device_fd_ < 0)
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device failed!", device_path_.c_str());
        return false;
    }
    else
    {
        std::thread(std::bind(&key_device::run, this)).detach();
    }
    return true;
}

void key_device::run()
{
    int flags;
    struct input_event event;

    PRINT_LOG(LOG_INFO, 0, "key device run start!");

    while (1)
    {
        flags = read(device_fd_, &event, sizeof(event));
        if (flags > 0) 
        {
            switch (event.type) 
            {
                case EV_KEY:
                        for (auto key_action:key_action_list)
                        {
                            if (event.code == key_action.key_num
                            && event.value == key_action.key_event)
                            {
                                key_action.func(key_action.key_num, key_action.key_event);
                                break;
                            }
                        }
                    break;
                case EV_REL:
                    break;
                case EV_ABS:
                    break;
                case EV_MSC:
                    break;
                case EV_SW:
                    break;
            }
        }
        else 
        {
            printf("read data failed!\n");
        }
    }
}

bool key_device::register_func(uint16_t key_num, uint16_t key_event, std::function<void(uint16_t, uint16_t)> func)
{
    if (device_fd_ >= 0)
    {
        KEY_ACTION_STRUCT key_action =
        {
            .key_num = key_num, 
            .key_event = key_event, 
            .func = func
        };

        key_action_list.push_back(key_action);
        PRINT_LOG(LOG_INFO, 0, "key register, totol:%d!", key_action_list.size());
        return true;
    }
    return false;
}