
#include "TimeManage.hpp"
#include "logger.hpp"
#include "rtc.hpp"

TimeManage *TimeManage::pInstance = nullptr;

TimeManage *TimeManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) TimeManage();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "TimeManage new error!");
        }
    }
    return pInstance;
}

void TimeManage::run()
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

bool TimeManage::registerWork(uint32_t id, uint32_t time, uint32_t count, std::function<void()> func)
{
    auto iter = VecWork.begin();  
    TimeAction work(id, time, count, func);   
    
    std::lock_guard<std::mutex> lock{mt};
    VecWork.push_back(work);

    return true;
}

bool TimeManage::removeWork(uint32_t id)
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

bool TimeManage::init(uint32_t timeInterval)
{
    peroid = timeInterval;
    time.start(timeInterval, std::bind(&TimeManage::run, this));

    return true;
}

uint32_t xGetCurrentTicks(void)
{
    return TimeManage::getInstance()->get_current_ticks();
}