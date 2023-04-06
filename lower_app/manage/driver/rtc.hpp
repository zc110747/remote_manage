//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      rtc.hpp
//
//  Purpose:
//      rtc时钟, 支持访问本地和硬件时钟
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

#include "device_base.hpp"
#include <linux/rtc.h>

class RTCDevice:public device_base
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

