//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      led.cpp
//
//  Purpose:
//      LED管理驱动，包含IO支持特性
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
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

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "LED On/Off 1s");
    ret &= on();
    sleep(1);
    ret &= off();
    sleep(1);

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "LED Trigger 1s, 5 times");
    for(int index=0; index<5; index++)
    {
        ret &= trigger();
        sleep(1);
    }

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "led test %s", ret?"success":"false");
}