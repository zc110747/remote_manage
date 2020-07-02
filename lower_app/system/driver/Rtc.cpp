/*
 * File      : Rtc.cpp
 * Rtc应用层驱动实现
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include "Rtc.h"
#include <time.h>

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static struct SSystemConfig *pSystemConfigInfo;
static int rtc_fd;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
/**
 * 配置RTC的驱动初始化
 * 
 * @param NULL
 *  
 * @return NULL
 */
void RtcDriveInit(void)
{
    pSystemConfigInfo = GetSSytemConfigInfo();

#if __WORK_IN_WSL == 0
    rtc_fd = open(pSystemConfigInfo->m_dev_rtc.c_str(), O_RDONLY);
    if(rtc_fd == -1)
    {
        DRIVER_DEBUG("rtc open %s failed!\n", pSystemConfigInfo->m_dev_rtc.c_str());
    }
#endif
}

/**
 * 释放RTC应用资源
 * 
 * @param NULL
 *  
 * @return NULL
 */
void RtcDriverRelease(void)
{
    close(rtc_fd);
}

/**
 * 读取系统当前的时钟状态信息
 * 
 * @param pRtcTime 获取的RTC结果值
 *  
 * @return 返回RTC读取结果的处理状态
 */
int RtcDevRead(struct rtc_time *pRtcTime)
{
//桌面端测试不包含rtc,使用系统时钟替代
#if __WORK_IN_WSL == 0
    int retval;

    if(rtc_fd != -1)
    {
        retval = ioctl(rtc_fd, RTC_RD_TIME, pRtcTime);
        if(retval == -1)
        {
            USR_DEBUG("Read %s Failed, error:%s\n", pSystemConfigInfo->m_dev_rtc.c_str(), strerror(errno));
            return RT_INVALID;
        }
    }
    else
    {
        return RT_INVALID;
    }
#else
    time_t timep;
    struct tm mytime, *p;

    time(&timep);
    p = localtime_r(&timep, &mytime); //gmtime_r将秒数转换成UTC时钟的时区值

    pRtcTime->tm_sec = p->tm_sec;
    pRtcTime->tm_min = p->tm_min;
    pRtcTime->tm_hour = p->tm_hour;
#endif

    return RT_OK;
}