//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      ProductConfig.hpp
//
//  Purpose:
//      包含应用配置信息的文件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

//------------------------- Global Defined --------------------------
#define UART_MODULE_ON                  1
#define SOCKET_TCP_MODULE_ON            1
#define SOCKET_UDP_MODULE_ON            1

/*返回状态*/
#define RT_INVALID_MQ_SIZE              -4
#define RT_INVALID_BUF_SIZE             -3
#define RT_INVALID_MQ                   -2
#define RT_OK               	        0
#define RT_FAIL             	        1
#define RT_EMPTY            	        2
#define RT_TIMEOUT                      3
#define RT_INVALID                      4

//默认设备ID
#define DEVICE_ID                       0x01

/*****************Default System Config******************/
#define SYSTM_CONFIG_ON             1
#define SYSTM_CONFIG_OFF            0
#define DEFAULT_CONFIG_FILE         "config.json"

#define DEFAULT_LED_INIT            SYSTM_CONFIG_OFF
#define DEFALUT_LED_DEV             "/dev/led"        
#define DEFAULT_BEEP_INIT           SYSTM_CONFIG_OFF
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
#define DEFAULT_LOGGER_IPADDR       DEFAULT_IPADDRESS
#define DEFAULT_LOGGER_PORT         8002
#define DEFAULT_NODE_IPADDR         DEFAULT_IPADDRESS
#define DEFAULT_NODE_PORT           8003
#define DEFAULT_DOWNLOAD_PATH       "/tmp/download/"

