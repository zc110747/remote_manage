//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver.hpp
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
_Pragma("once")

#include "led.hpp"
#include "beep.hpp"
#include "i2c_ap.hpp"
#include "spi_icm.hpp"
#include "rtc.hpp"

class DriverManage
{
private:
    static DriverManage* pInstance;

public:
    DriverManage() = default;
    ~DriverManage() = delete;
    bool init();
    void release();
    static DriverManage* getInstance();
};

