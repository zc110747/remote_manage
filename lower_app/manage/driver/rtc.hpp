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
//      ZhangChao
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

public:
    //constructor
    using deviceBase::deviceBase;

#if __WORK_IN_WSL
    bool open(int flag){
        //do nothing
        return true;
    }
#endif

    bool updateTime();
    static RTCDevice* getInstance();
    void release();

    struct rtc_time* getRtcTime() {return &rtcTimeM;}
};
#endif
