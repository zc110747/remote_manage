//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      key.cpp
//
//  Purpose:
//      按键管理模块,处理按键的初始化和注册回调事件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/24/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////

#include <map>
#include "key.hpp"
#include "logger.hpp"
#include "includes.hpp"

static std::map<int, std::function<void(int)>> FuncList;

static void sigio_signal_func(int signum)
{
    for(auto &&func : FuncList)
    {
        func.second(func.first);
    }
}

bool KEY::init(const std::string &DevicePath, int flags)
{
    static bool is_first_run = true;
    devicePathM = DevicePath;

    DeviceFdM = ::open(devicePathM.c_str(), flags);
    if(DeviceFdM < 0)
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device failed!", devicePathM.c_str());
        return false;
    }
    else
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device success, fd:%d!", devicePathM.c_str(), DeviceFdM);

        if(is_first_run)
        {
            PRINT_LOG(LOG_INFO, 0, "sigio register!");
            signal(SIGIO, sigio_signal_func);
            is_first_run = false;
        }
        fcntl(DeviceFdM, F_SETOWN, getpid());		/* 设置当前进程接收SIGIO信号 	*/
        flags = fcntl(DeviceFdM, F_GETFL);			/* 获取当前的进程状态 			*/
        fcntl(DeviceFdM, F_SETFL, flags | FASYNC);	/* 设置进程启用异步通知功能 	*/	
    }
    return true;
}

bool KEY::register_func(std::function<void(int)> func)
{
    if(DeviceFdM >= 0)
    {
        FuncList.insert(std::make_pair(DeviceFdM, func));
        PRINT_LOG(LOG_INFO, 0, "key register, totol:%d!", FuncList.size());
        return true;
    }
    return false;
}