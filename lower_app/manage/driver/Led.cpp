/*
 * File      : ledBase.cpp
 * ledBase应用层驱动实现
 * COPYRIGHT (C) 2020, zc
 */
#include "led.hpp"
#include "../include/SystemConfig.h"

ledTheOne* ledTheOne::pInstance = nullptr;
ledTheOne* ledTheOne::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) ledTheOne(static_cast<SSystemConfig *>(GetSSytemConfigInfo())->m_dev_led);
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