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

driver_manage* driver_manage::pInstance = nullptr;
driver_manage* driver_manage::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) driver_manage;
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

bool driver_manage::init()
{
    bool ret = true;
    system_config *pConfig = system_config::getInstance();

    ret &= led_0.init(pConfig->getled()->dev, O_RDWR | O_NONBLOCK);
    ret &= beep_0.init(pConfig->getbeep()->dev, O_RDWR | O_NONBLOCK);
    ret &= ap3216_dev_.init(pConfig->getapI2c()->dev, O_RDONLY | O_NONBLOCK);
    ret &= icm20608_dev_.init(pConfig->geticmSpi()->dev, O_RDONLY | O_NONBLOCK);
    ret &= RTCDevice::getInstance()->open(O_RDONLY | O_NONBLOCK);
    ret &= key_0.init(pConfig->getkey()->dev, O_RDWR | O_NONBLOCK);

    if(ret)
    {
        led_0.writeIoStatus(pConfig->getled()->init);
        beep_0.writeIoStatus(pConfig->getbeep()->init);
        PRINT_LOG(LOG_INFO, 0, "Device driver_manage Init Success!");
    }
    
    return ret;
}

void driver_manage::release()
{
    RTCDevice::getInstance()->release();
}


	