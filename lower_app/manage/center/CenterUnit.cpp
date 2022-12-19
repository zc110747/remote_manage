//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CenterUnit.cpp
//
//  Purpose:
//      核心管理模块，用于接收所有模块的数据，进行管理并分发
//      输入:网络通讯模块, QT界面事件，node输入事件, 设备状态更新，logger调试接口
//      输出：本地硬件设备
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
#include "CenterUnit.hpp"
#include "logger.hpp"
#include <new>

CenterUnit* CenterUnit::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) CenterUnit;
        if(pInstance == nullptr)
        {
             PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "DeviceManageThread new error!");
        }
    }
    return pInstance;
}

void CenterUnit::init()
{
    
}