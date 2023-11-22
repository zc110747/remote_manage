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

#define PROCESS_RUN                 PROCESS_LOCAL_DEVICE

//------------------------- FIFO List ------------------------------
#define LOGGER_FIFO_PATH                "/tmp/loc_logger.fifo"              //logger服务模块
#define DEVICE_MESSAGE_FIFO             "/tmp/device_manage.fifo"        //设备管理模块
#define ASIO_CLENET_FIFO                "/tmp/loc_client_tx.fifo"

//默认设备ID
#define DEVICE_ID                       0x01

