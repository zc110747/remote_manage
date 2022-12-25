//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver.cpp
//
//  Purpose:
//      驱动管理模块，用于初始化开启所有硬件使用的外设
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
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
    SystemConfig *pConfig = SystemConfig::getInstance();

    ret &= led_0.init(pConfig->getled()->dev, O_RDWR | O_NDELAY);
    ret &= beep_0.init(pConfig->getbeep()->dev, O_RDWR | O_NDELAY);
    ret &= ap_dev_0.init(pConfig->getapI2c()->dev, O_RDONLY);
    ret &= icm_dev_0.init(pConfig->geticmSpi()->dev, O_RDONLY);
    ret &= RTCDevice::getInstance()->open(O_RDONLY);
    ret &= key_0.init(pConfig->getkey()->dev, O_RDWR | O_NDELAY);

    if(ret)
    {
        led_0.writeIoStatus(pConfig->getled()->init);
        beep_0.writeIoStatus(pConfig->getbeep()->init);
        PRINT_LOG(LOG_INFO, 0, "Device DriverManage Init Success!");
    }
    
    return ret;
}

void DriverManage::release()
{
    RTCDevice::getInstance()->release();
}


	