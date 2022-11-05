//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      DriverManage.cpp
//
//  Purpose:
//      DriverManage init and release.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "driver.hpp"

DriverManage* DriverManage::pInstance = nullptr;
DriverManage* DriverManage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) DriverManage;
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

bool DriverManage::init()
{
    bool ret = true;

    ret &= ledTheOne::getInstance()->open(O_RDWR | O_NDELAY);
    ret &= beepTheOne::getInstance()->open(O_RDWR | O_NDELAY);
    ret &= APDevice::getInstance()->open(O_RDONLY);
    ret &= ICMDevice::getInstance()->open(O_RDONLY);
    ret &= RTCDevice::getInstance()->open(O_RDONLY);

    if(ret)
    {
        ledTheOne::getInstance()->writeIoStatus(SystemConfig::getInstance()->getled()->init);
        beepTheOne::getInstance()->writeIoStatus(SystemConfig::getInstance()->getbeep()->init);
        PRINT_LOG(LOG_ERROR, 0, "Device DriverManage Init Success!");
    }
    
    return ret;
}

void DriverManage::release()
{
    ledTheOne::getInstance()->release();
    beepTheOne::getInstance()->release();
    APDevice::getInstance()->release();
    ICMDevice::getInstance()->release();
    RTCDevice::getInstance()->release();
}


	