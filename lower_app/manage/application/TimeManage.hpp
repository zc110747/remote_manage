_Pragma("once")

#include "tools/timer.hpp"
#include <new>
#include <vector>
#include <mutex>

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

    bool init(int timeval = TIME_INTERVAL);
    bool removeWork(int id);
    bool registerWork(int id, uint32_t time, uint32_t count, std::function<void()> func);
private:
    void run();
    static TimeManage *pInstance;
    DeviceManage::Timer time;
    std::vector<TimeAction> VecWork;
    std::mutex mt;
};