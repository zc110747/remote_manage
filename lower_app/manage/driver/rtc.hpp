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

class rtc_device:public device_base
{
public:
    // - constructor
    using device_base::device_base;

    bool update_rtc_time();

    int get_current_second();

    bool init(const std::string &DevicePath, int flags);

    struct rtc_time* getRtcTime() {return &rtcTimeM;}
    uint64_t getStartTime() {return TimeStart;}

private:
    struct rtc_time rtcTimeM{0};
    uint64_t TimeStart{0};
};

