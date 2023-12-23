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
            ret = true;
    }
#endif
    return ret;
}

bool rtc_device::init(const std::string &DevicePath, int flags)
{
    start_time_ = get_current_time();

    return device_base::init(DevicePath, flags);
}

int rtc_device::get_current_time()
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

