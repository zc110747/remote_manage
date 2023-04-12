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
#include "key.hpp"

class driver_manage
{
private:
    static driver_manage* pInstance;
    LED led_0;
    BEEP beep_0;
    ap_device ap3216_dev_;
    icm_device icm20608_dev_;
    KEY key_0;

public:
    driver_manage() = default;
    ~driver_manage() = delete;
    bool init();
    void release();
    static driver_manage* getInstance();

public:
    LED *getLed0()          {return &led_0;}
    BEEP *getBeep0()        {return &beep_0;}
    ap_device *get_ap3126_dev()   {return &ap3216_dev_;}
    icm_device *getIcmDev0() {return &icm20608_dev_;}
    KEY *getKey0()          {return &key_0;}
};

