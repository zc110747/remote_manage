/*
 * File      : template.cpp
 * template file
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
 * RTC驱动需要初始化的信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
void RtcDriveInit(void)
{
    pSystemConfigInfo = GetSSytemConfigInfo();
}

/**
 * RTC读取参数值
 * 
 * @param pRtcTime 获取的RTC结果值
 *  
 * @return 返回RTC读取结果的处理状态
 */
int RtcDevRead(struct rtc_time *pRtcTime)
{
//桌面端测试不包含rtc,使用系统时钟替代
#if __WORK_IN_WSL == 0
    int rtc_fd;
    int retval;

    rtc_fd = open(pSystemConfigInfo->m_dev_rtc.c_str(), O_RDONLY);
    if(rtc_fd < 0){
        //USR_DEBUG("Open %s Failed, error:%s\n", pSystemConfigInfo->m_dev_rtc.c_str(), strerror(errno));
        return RT_INVALID;
    }

    retval = ioctl(rtc_fd, RTC_RD_TIME, pRtcTime);
    if(retval == -1)
    {
        USR_DEBUG("Read %s Failed, error:%s\n", pSystemConfigInfo->m_dev_rtc.c_str(), strerror(errno));
        return RT_INVALID;
    }
    close(rtc_fd);
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