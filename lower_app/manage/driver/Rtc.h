/*
 * File      : Rtc.h
 * Rtc应用层驱动接口
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
#ifndef _INCLUDE_RTC_H
#define _INCLUDE_RTC_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../include/productConfig.hpp"
#include "../include/SystemConfig.h"
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
/*配置RTC的驱动初始化*/
void RtcDriveInit(void);

/*释放RTC应用资源*/
void RtcDriverRelease(void);

/*读取系统当前的时钟状态信息*/
int RtcDevRead(struct rtc_time *pRtcTime);
#endif
