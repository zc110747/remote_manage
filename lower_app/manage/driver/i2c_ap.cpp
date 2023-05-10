//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      i2c_ap.cpp
//
//  Purpose:
//      AP模块对应的I2C接口，通过继承支持直接读取模块信息
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
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