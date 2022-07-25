//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      productConfig.hpp
//
//  Purpose:
//      product default config.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _PRODUCTCONFIG_H
#define _PRODUCTCONFIG_H

#include "includes.hpp"

//------------------------- Global Defined --------------------------
#define DEVICE_VERSION          "0.0.0.0"

#define UART_MODULE_ON          1
#define SOCKET_TCP_MODULE_ON    1
#define SOCKET_UDP_MODULE_ON    1

/*自定义协议应用测试*/
#define __SYSTEM_DEBUG          0
#define __WORK_IN_WSL           1  //在WSL中，Posix Mq不支持，改为FIFO方案

/*调试打印口显示*/
#define __DEBUG_PRINTF			1
#define __DRIVER_DEBUG          1
#define __SOCKET_DEBUG          1

/*返回状态*/
#define RT_INVALID_MQ_SIZE      -4
#define RT_INVALID_BUF_SIZE     -3
#define RT_INVALID_MQ           -2
#define RT_OK               	0
#define RT_FAIL             	1
#define RT_EMPTY            	2
#define RT_TIMEOUT              3
#define RT_INVALID              4

//默认设备ID
#define DEVICE_ID                   0x01

//default system config set
#define SYSTM_CONFIG_ON             1
#define SYSTM_CONFIG_OFF            0

#define DEFAULT_CONFIG_FILE         "config.json"

#define DEFAULT_LED_INIT            SYSTM_CONFIG_ON
#define DEFALUT_LED_DEV             "/dev/led"        

#define DEFAULT_BEEP_INIT           SYSTM_CONFIG_ON
#define DEFALUT_BEEP_DEV            "/dev/beep"   

#define DEFAULT_SERIAL_BAUD         115200
#define DEFAULT_SERIAL_DATABITS     8
#define DEFAULT_SERIAL_STOPBITS     1
#define DEFAULT_SERIAL_PARITY       "n"
#define DEFAULT_SERIAL_DEV          "/dev/ttymxc2"

#define DEFAULT_RTC_DEV             "/dev/rtc0"
#define DEFAULT_ICMSPI_DEV          "/dev/icm20608"
#define DEFAULT_API2C_DEV           "/dev/ap3216"

#define DEFAULT_IPADDRESS           "127.0.0.1"

#define DEFAULT_TCP_IPADDR          DEFAULT_IPADDRESS
#define DEFAULT_TCP_PORT            8000
#define DEFAULT_UDP_IPADDR          DEFAULT_IPADDRESS
#define DEFAULT_UDP_PORT            8001

#define DEFAULT_DOWNLOAD_PATH       "/home/manage/download/"

/*调试接口*/
#if __DEBUG_PRINTF	== 1
#define USR_DEBUG		printf
#else
static int USR_DEBUG(const char *format, ...){}
#endif

#if __DRIVER_DEBUG == 1
#define DRIVER_DEBUG		USR_DEBUG
#else
static int DRIVER_DEBUG(const char *format, ...){}
#endif

#if __SOCKET_DEBUG == 1
#define SOCKET_DEBUG        USR_DEBUG
#else
static int SOCKET_DEBUG(const char *format, ...){}
#endif

void SystemLogArray(uint8_t *pArrayBuffer, uint16_t nArraySize);
#endif
