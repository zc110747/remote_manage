//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      CenterUnit.hpp
//
//  Purpose:
//      用于管理整个数据通讯的核心线程
//
// Author:
//     	Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
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