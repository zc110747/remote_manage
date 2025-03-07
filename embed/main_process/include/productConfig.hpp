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

#include "global_def.hpp"

#define PROCESS_RUN                 PROCESS_MAIN_PROCESS
#define TOOLS_NAME                  "main_process"
#define PRINT_NOW_HEAD_STR          "[0000 00:00:00][main_process][5]"

//------------------------- FIFO List ------------------------------
#define DEVICE_MESSAGE_FIFO         "/tmp/device_manage.fifo"        //设备管理模块
#define CENTER_UNIT_FIFO            "/tmp/center_manage.fifo"

//socket fifo
#define SOCKET_TCP_RX_FIFO          "/tmp/socket_rx.fifo"
#define SOCKET_TCP_TX_FIFO          "/tmp/socket_tx.fifo"

//uart fifo
#define SERVER_UART_RX_FIFO          "/tmp/uart_rx.fifo"
#define SERVER_UART_TX_FIFO          "/tmp/uart_tx.fifo"

//默认设备ID
#define DEVICE_ID                       0x01
