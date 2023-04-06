//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      time_manage.cpp
//
//  Purpose:
//      时间管理模块, 支持注册周期性触发的事件
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "time_manage.hpp"
#include "logger.hpp"

time_manage *time_manage::pInstance = nullptr;

time_manage *time_manage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) time_manage();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "time_manage new error!");
        }
    }
    return pInstance;
}

void time_manage::run()
{
    auto iter = VecWork.begin();  

    //计数
    ticks += peroid;

    std::lock_guard<std::mutex> lock{mt};
    while(iter != VecWork.end())
    {
        if(iter->ActionCount == 0)
        {
            iter = VecWork.erase(iter);
        }
        else
        {
            iter->action();
            iter++;
        }
    }
}

bool time_manage::registerWork(uint32_t id, uint32_t time, uint32_t count, std::function<void()> func)
{
    auto iter = VecWork.begin();  
    TimeAction work(id, time, count, func);   
    
    std::lock_guard<std::mutex> lock{mt};
    VecWork.push_back(work);

    return true;
}

bool time_manage::removeWork(uint32_t id)
{
    auto iter = VecWork.begin();  

    std::lock_guard<std::mutex> lock{mt};
    while(iter != VecWork.end())
    {
        if(iter->Id == id)
        {
            iter = VecWork.erase(iter);
        }
        else
        {
            iter++;
        }
    }

    return true;
}

bool time_manage::init(uint32_t timeInterval)
{
    peroid = timeInterval;
    time.start(timeInterval, std::bind(&time_manage::run, this));

    return true;
}

uint32_t xGetCurrentTicks(void)
{
    return time_manage::getInstance()->get_current_ticks();
}