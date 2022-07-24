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
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
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