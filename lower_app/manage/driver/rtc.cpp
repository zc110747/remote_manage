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

RTCDevice::RTCDevice(const std::string &DevicePath)
:device_base(DevicePath)
{
    TimeStart = getCurrentSecond();
}

RTCDevice::~RTCDevice()
{

}

RTCDevice* RTCDevice::pInstance = nullptr;
RTCDevice* RTCDevice::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) RTCDevice(system_config::getInstance()->getrtc()->dev);
        if(pInstance == NULL)
        {
            //To Do something(may logger)
        }
    }
    return pInstance;
}

void RTCDevice::release()
{
    if(pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}

bool RTCDevice::updateTime()
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

    if(DeviceFdM>=0)
    {
        retval = ioctl(DeviceFdM, RTC_RD_TIME, &rtcTimeM);
        if(retval >= 0)
            ret = true;
    }
#endif
    return ret;
}

int RTCDevice::getCurrentSecond()
{
    uint64_t second;

    updateTime();
    
    second += rtcTimeM.tm_hour*3600;
    second += rtcTimeM.tm_min*60;
    second += rtcTimeM.tm_sec;
    return second-TimeStart;
}

