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

#define PROCESS_RUN                     PROCESS_LOWER_DEVICE

//------------------------- FIFO List ------------------------------
#define LOGGER_FIFO_PATH                "/tmp/ld_logger.fifo"              //logger服务模块

//uart fifo
#define SERVER_UART_RX_FIFO             "/tmp/uart_rx.fifo"
#define SERVER_UART_TX_FIFO             "/tmp/uart_tx.fifo"

#define ASIO_CLENET_FIFO                "/tmp/ld_client_tx.fifo"

//默认设备ID
#define DEVICE_ID                       0x01


