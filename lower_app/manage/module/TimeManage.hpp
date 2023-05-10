//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      TimeManage.hpp
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
_Pragma("once")

#include "timer.hpp"
#include <new>
#include <vector>
#include <mutex>
#include <atomic>

#define TIME_ACTION_ALWAYS      0xFFFFFFF
#define TIME_INTERVAL           100
#define TIME_TICK(ms)           ((ms)/TIME_INTERVAL)

class TimeAction
{
public:
    TimeAction(int id, uint32_t time, uint32_t count, std::function<void()> func)
    :Id(id), TickCompare(time), Func(func), ActionCount(count){
        TickCount = 0;
    }
    ~TimeAction(){}

    bool isMatch()
    {
        TickCount++;

        if(TickCount >= TickCompare)
        {
            TickCount = 0;
            return true;
        }
        else
        {
            return false;
        }
    }

    void action()
    {
        if(ActionCount >0 && isMatch())
        {
            if(ActionCount != TIME_ACTION_ALWAYS)
            {
                ActionCount--;
            }

            Func();
        }
    }

public:
    uint32_t Id;
    uint32_t ActionCount;
    
private:
    std::function<void()> Func;
    uint32_t TickCount;
    uint32_t TickCompare;
};

class TimeManage
{
public:
    static TimeManage *getInstance();
    TimeManage(): ticks(0){
    }

    bool init(uint32_t timeInterval = TIME_INTERVAL);
    bool removeWork(uint32_t id);
    bool registerWork(uint32_t id, uint32_t time, uint32_t count, std::function<void()> func);
    uint32_t get_current_ticks() {return ticks;}
private:
    void run();
    static TimeManage *pInstance;
    DeviceManage::Timer time;
    std::vector<TimeAction> VecWork;
    std::mutex mt;
    std::atomic<uint32_t> ticks;
    uint32_t peroid;
};
