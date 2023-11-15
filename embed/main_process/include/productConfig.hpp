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

//------------------------- FIFO List ------------------------------
#define LOGGER_FIFO_PATH            "/tmp/logger.fifo"              //logger服务模块
#define DEVICE_MESSAGE_FIFO         "/tmp/device_manage.fifo"        //设备管理模块
#define CENTER_UNIT_FIFO            "/tmp/center_manage.fifo"

//socket fifo
#define SOCKET_TCP_RX_FIFO          "/tmp/socket_rx.fifo"
#define SOCKET_TCP_TX_FIFO          "/tmp/socket_tx.fifo"

//uart fifo
#define SERVER_UART_RX_FIFO          "/tmp/uart_rx.fifo"
#define SERVER_UART_TX_FIFO          "/tmp/uart_tx.fifo"

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

#define DEFAULT_CONFIG_FILE             "config.json"
