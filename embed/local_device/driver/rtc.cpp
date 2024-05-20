//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      rtc.cpp
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
#include <sys/ioctl.h>
#include "rtc.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

bool rtc_device::update_rtc_time()
{
    bool ret = false;

#if __WORK_IN_WSL
    time_t timep;
    struct tm mytime, *p;

    time(&timep);
    p = localtime_r(&timep, &mytime); //gmtime_r将秒数转换成UTC时钟的时区值

    if (p != NULL)
    {
        rtc_time_.tm_sec = p->tm_sec;
        rtc_time_.tm_min = p->tm_min;
        rtc_time_.tm_hour = p->tm_hour;
        ret = true;
    }
#else
    int retval;

    if (device_fd_>=0)
    {
        retval = ioctl(device_fd_, RTC_RD_TIME, &rtc_time_);
        if (retval >= 0)
        {
            time_str_ = fmt::format("{0:0>4}:{1:0>2}:{2:0>2} {3:0>2}:{4:0>2}:{5:0>2}", 
                                    rtc_time_.tm_year, rtc_time_.tm_mon, rtc_time_.tm_mday,
                                    rtc_time_.tm_hour, rtc_time_.tm_min, rtc_time_.tm_sec);
            ret = true;
        }

    }
#endif
    return ret;
}

bool rtc_device::init(const std::string &DevicePath, int flags)
{
    start_time_ = get_run_time();

    return device_base::init(DevicePath, flags);
}

int rtc_device::get_run_time()
{
    uint64_t second = 0;

    if (update_rtc_time())
    {
        second = rtc_time_.tm_hour*3600;
        second += rtc_time_.tm_min*60;
        second += rtc_time_.tm_sec;
    }
    return second-start_time_;
}

std::string &rtc_device::get_timer_str()
{
    return time_str_;
}