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

driver_manage* driver_manage::instance_pointer_ = nullptr;
driver_manage* driver_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) driver_manage;
        if (instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

bool driver_manage::init()
{
    bool ret = true;
    
    system_config *pConfig = system_config::get_instance();

    ret &= led_zero_.init(pConfig->get_led_config().dev, O_RDWR | O_NONBLOCK);
    ret &= beep_zero_.init(pConfig->get_beep_config().dev, O_RDWR | O_NONBLOCK);
    ret &= ap3216_dev_.init(pConfig->get_ap_config().dev, O_RDONLY | O_NONBLOCK);
    ret &= icm20608_dev_.init(pConfig->get_icm_config().dev, O_RDONLY | O_NONBLOCK);
    ret &= rtc_dev_.init(pConfig->get_rtc_config().dev,  O_RDONLY | O_NONBLOCK);
    ret &= key_zero_.init(pConfig->get_key_config().dev, O_RDWR);

    if (ret)
    {
        led_zero_.write_io_status(pConfig->get_led_config().init);
        beep_zero_.write_io_status(pConfig->get_beep_config().init);
        PRINT_LOG(LOG_INFO, 0, "Device driver_manage Init Success!");
    }
    
    return true;
}
