//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mb_app.cpp
//
//  Purpose:
//      本项目作为嵌入式管理平台的下位机，主要负责网络，串口通讯，本地设备管理，
//远端设备轮询。
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
//
/////////////////////////////////////////////////////////////////////////////
#include "mb_app.hpp"
#include "mb.h"
#include "mbport.h"
#include "common_unit.hpp"

/* mb_tcp 格式 */
/* 
1. 读线圈状态(0x01)
命令: 00 00 00 00 00 06 01 01 00 00 00 20
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 数据长度(2byte) |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06   | 0x01           | 0x01          | 0x00 0x00      | 00 20          |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 字节计数(1byte) |  数据          |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x07   | 0x01           | 0x01          | 0x04           | 10 01 01 00    |

2. 写单个线圈(0x05)
命令: 00 00 00 00 00 06 01 05 00 00 FF 00
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06   | 0x01           | 0x05          | 0x00 0x00      | FF 00 |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06  | 0x01           | 0x05          | 0x00 0x00      | FF 00 |

3. 写多个线圈(0x0F)
命令: 00 00 00 00 00 09 01 0F 00 00 00 10 02 1C A0
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 线圈数量(2byte) | 字节计数(1byte) | 写入值 |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x09   | 0x01           | 0x0F          | 0x00 0x00      | 0x00 0x10      |  0x02          | 1C A0 |
应答:
| 事务处理标识(2byte) | 协议标识(2byte) | 长度(2byte) | 单元标识(1byte) | 功能码(1byte) | 起始地址(2byte) | 线圈数量(2byte) |
| 0x00 0x00          | 0x00 0x00      | 0x00 0x06  | 0x01            | 0x0F          | 0x00 0x00       | 00 10         |
*/

#define MODBUS_RUN_MODE             0
#define MODBUS_DEF_ADDRESS          0x01        //从设备地址
#define MODBUS_DEF_UBAUD            115200      //设备波特率
#define MODBUS_DEF_PARITY           MB_PAR_NONE //奇偶校验位

#define MODBUS_TCP_PORT             8080

void mb_app_manage::mb_workflow_run()
{
    eMBErrorCode eStatus;

#if MODBUS_RUN_MODE == 0    
    eStatus = eMBInit( MB_RTU, MODBUS_DEF_ADDRESS, 0, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR ) {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "eMBInit RTU Mode Failed, ErroCode:%d!", eStatus);
        return;
    }
#elif MODBUS_RUN_MODE == 1
    eStatus = eMBInit( MB_ASCII, MODBUS_DEF_ADDRESS, 0, MODBUS_DEF_UBAUD, MODBUS_DEF_PARITY );
    if( eStatus != MB_ENOERR ) {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "eMBInit ASCII Mode Failed, ErroCode:%d!", eStatus);
        return;
    }
#elif MODBUS_RUN_MODE == 2
    eStatus = eMBTCPInit(MODBUS_TCP_PORT);
    if( eStatus != MB_ENOERR ) {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "eMBInit TCP Mode Failed, ErroCode:%d!", eStatus);
        return;
    }
#endif

    eStatus = eMBEnable(  );
    if( eStatus != MB_ENOERR ) {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "eMBInit Enabled Failed, ErroCode:%d!", eStatus);
        return;
    }

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "eMBInit Init and Enable success!");

    while(1)
    {
        eStatus = eMBPoll(  );

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

bool mb_app_manage::init()
{
    mb_workflow_thread_ = std::thread(std::bind(&mb_app_manage::mb_workflow_run, this));
    mb_workflow_thread_.detach();

    return true;
}

mb_app_manage* mb_app_manage::instance_pointer_ = nullptr;
mb_app_manage* mb_app_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) mb_app_manage;
        if (instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "mp manage init failed!");
        }
    }
    return instance_pointer_;
}