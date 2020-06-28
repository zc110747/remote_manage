/*
 * File      : Rtc.h
 * This file is for RTC Interface
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
#include "../include/UsrTypeDef.h"
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
void RtcDriveInit(void);
int RtcDevRead(struct rtc_time *pRtcTime);
#endif
