//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      rtc.hpp
//
//  Purpose:
//      rtc device interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_RTC_HPP
#define _INCLUDE_RTC_HPP

#include "deviceBase.hpp"
#include <linux/rtc.h>

class RTCDevice:public deviceBase
{
private:
    struct rtc_time rtcTimeM{0};
    static RTCDevice* pInstance;
    uint64_t TimeStart{0};

public:
    RTCDevice(const std::string &DevicePath);
    ~RTCDevice();

    bool updateTime();
    static RTCDevice* getInstance();
    void release();

    int getCurrentSecond();
    struct rtc_time* getRtcTime() {return &rtcTimeM;}
    uint64_t getStartTime() {return TimeStart;}
};

#endif
