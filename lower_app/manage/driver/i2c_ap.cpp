//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      i2c_ap.cpp
//
//  Purpose:
//      i2c read ap application.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "i2c_ap.hpp"

APDevice* APDevice::pInstance = nullptr;
APDevice* APDevice::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) APDevice(SystemConfig::getInstance()->getapI2c()->dev);
        if(pInstance == NULL)
        {
            //To Do something(may logger)
        }
    }
    return pInstance;
}

void APDevice::release()
{
    if(pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}