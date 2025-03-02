//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      timer_manage.cpp
//
//  Purpose:
//      时间管理组件
//      1.提供系统定时器，通过接口xGetCurrentTicks获取当前工作的时间计数
//      提供非精确的延时处理，精度位10
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "timer_manage.hpp"
#include "logger_manage.hpp"

timer_manage *timer_manage::instance_pointer_ = nullptr;

timer_manage *timer_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) timer_manage();
        if (instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "timer_manage new error!");
        }
    }
    return instance_pointer_;
}

void timer_manage::run()
{
    auto iter = time_action_vec_.begin();  

    //计数
    ticks_ += peroid_;

    std::lock_guard<std::mutex> lock{mutex_};
    while (iter != time_action_vec_.end())
    {
        if (iter->get_action_times() == 0)
        {
            iter = time_action_vec_.erase(iter);
        }
        else
        {
            iter->action();
            iter++;
        }
    }
}

bool timer_manage::register_action(uint32_t id, uint32_t time, uint32_t count, std::function<void(int id)> func)
{
    auto iter = time_action_vec_.begin();  
    timer_action work(id, time, count, func);   
    
    std::lock_guard<std::mutex> lock{mutex_};
    time_action_vec_.push_back(work);

    return true;
}

bool timer_manage::remove_action(uint32_t id)
{
    auto iter = time_action_vec_.begin();  

    std::lock_guard<std::mutex> lock{mutex_};
    while (iter != time_action_vec_.end())
    {
        if (iter->get_event_id() == id)
        {
            iter = time_action_vec_.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    return true;
}

bool timer_manage::init(uint32_t timeInterval)
{
    peroid_ = timeInterval;
    
    timer_.start(timeInterval, std::bind(&timer_manage::run, this));

    PRINT_NOW("%s:timer_manage init success!\n", PRINT_NOW_HEAD_STR);
    return true;
}

uint32_t xGetCurrentTicks(void)
{
    return timer_manage::get_instance()->get_current_ticks();
}

uint32_t xGetCurrentPeroid(void)
{
    return timer_manage::get_instance()->get_current_period();
}

uint32_t xGetCurrentTimes(void)
{
    return xGetCurrentTicks()/1000;
}

