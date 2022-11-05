//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      led.cpp
//
//  Purpose:
//      led device.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "led.hpp"
#include "../logger/logger.hpp"
#include "../driver/rtc.hpp"

ledTheOne* ledTheOne::pInstance = nullptr;
ledTheOne* ledTheOne::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) ledTheOne(SystemConfig::getInstance()->getled()->dev);
        if(pInstance == NULL)
        {
            //To Do something(may logger)
        }
    }
    return pInstance;
}

void ledTheOne::release()
{
    if(pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

void ledTheOne::test()
{
    bool ret = true;

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "LED On/Off 1s");
    ret &= on();
    sleep(1);
    ret &= off();
    sleep(1);

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "LED Trigger 1s, 5 times");
    for(int index=0; index<5; index++)
    {
        ret &= trigger();
        sleep(1);
    }

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "led test %s", ret?"success":"false");
}