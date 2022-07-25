//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      rtc.cpp
//
//  Purpose:
//      rtc device.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "rtc.hpp"
#include <sys/ioctl.h>

RTCDevice* RTCDevice::pInstance = nullptr;
RTCDevice* RTCDevice::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) RTCDevice(static_cast<SSystemConfig *>(GetSSytemConfigInfo())->m_dev_rtc);
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




// /**************************************************************************
// * Local Macro Definition
// ***************************************************************************/

// /**************************************************************************
// * Local Type Definition
// ***************************************************************************/

// /**************************************************************************
// * Local static Variable Declaration
// ***************************************************************************/
// static struct SSystemConfig *pSystemConfigInfo;
// static int rtc_fd;

// /**************************************************************************
// * Global Variable Declaration
// ***************************************************************************/

// /**************************************************************************
// * Local Function Declaration
// ***************************************************************************/

// /**************************************************************************
// * Local Function
// ***************************************************************************/

// /**************************************************************************
// * Function
// ***************************************************************************/
// /**
//  * 配置RTC的驱动初始化
//  * 
//  * @param NULL
//  *  
//  * @return NULL
//  */
// void RtcDriveInit(void)
// {
//     pSystemConfigInfo = GetSSytemConfigInfo();

// #if __WORK_IN_WSL == 0
//     rtc_fd = open(pSystemConfigInfo->m_dev_rtc.c_str(), O_RDONLY);
//     if(rtc_fd == -1)
//     {
//         DRIVER_DEBUG("rtc open %s failed!\n", pSystemConfigInfo->m_dev_rtc.c_str());
//     }
// #endif
// }

// /**
//  * 释放RTC应用资源
//  * 
//  * @param NULL
//  *  
//  * @return NULL
//  */
// void RtcDriverRelease(void)
// {
//     close(rtc_fd);
// }

// /**
//  * 读取系统当前的时钟状态信息
//  * 
//  * @param pRtcTime 获取的RTC结果值
//  *  
//  * @return 返回RTC读取结果的处理状态
//  */
// int RtcDevRead(struct rtc_time *pRtcTime)
// {
// //桌面端测试不包含rtc,使用系统时钟替代
// #if __WORK_IN_WSL == 0
//     int retval;

//     if(rtc_fd != -1)
//     {
//         retval = ioctl(rtc_fd, RTC_RD_TIME, pRtcTime);
//         if(retval == -1)
//         {
//             USR_DEBUG("Read %s Failed, error:%s\n", pSystemConfigInfo->m_dev_rtc.c_str(), strerror(errno));
//             return RT_INVALID;
//         }
//     }
//     else
//     {
//         return RT_INVALID;
//     }
// #else

// #endif

//     return RT_OK;
// }