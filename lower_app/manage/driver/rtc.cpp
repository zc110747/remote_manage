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
#include "rtc.hpp"
#include <sys/ioctl.h>

bool rtc_device::update_rtc_time()
{
    bool ret = false;

#if __WORK_IN_WSL
    time_t timep;
    struct tm mytime, *p;

    time(&timep);
    p = localtime_r(&timep, &mytime); //gmtime_r将秒数转换成UTC时钟的时区值

    if(p != NULL)
    {
        rtcTimeM.tm_sec = p->tm_sec;
        rtcTimeM.tm_min = p->tm_min;
        rtcTimeM.tm_hour = p->tm_hour;
        ret = true;
    }
#else
    int retval;

    if(device_fd_>=0)
    {
        retval = ioctl(device_fd_, RTC_RD_TIME, &rtcTimeM);
        if(retval >= 0)
            ret = true;
    }
#endif
    return ret;
}

bool rtc_device::init(const std::string &DevicePath, int flags)
{
    device_base::init(DevicePath, flags);
    TimeStart = get_current_second();
}

int rtc_device::get_current_second()
{
    uint64_t second;

    if(update_rtc_time())
    {
        second += rtcTimeM.tm_hour*3600;
        second += rtcTimeM.tm_min*60;
        second += rtcTimeM.tm_sec;
    }

    return second-TimeStart;
}

