//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      key.cpp
//
//  Purpose:
//      KEY管理驱动，输入的硬件特性
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////

#include <vector>
#include "key.hpp"
#include "logger.hpp"
#include "includes.hpp"

static int fd;

static void sigio_signal_func(int signum)
{
	int err = 0;
	unsigned int keyvalue = 0;

	err = read(fd, &keyvalue, sizeof(keyvalue));
	if(err < 0) {
		/* 读取错误 */
	} else {
		printf("sigio signal! key value=%d\r\n", keyvalue);
	}
}

KEY *KEY::pInstance = nullptr;
KEY *KEY::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) KEY(SystemConfig::getInstance()->getkey()->dev);
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

bool KEY::open(int flags)
{
    DeviceFdM = ::open(devicePathM.c_str(), flags);
    if(DeviceFdM == -1)
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device failed!", devicePathM.c_str());
        return false;
    }
    else
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device success, fd:%d!", devicePathM.c_str(), DeviceFdM);
        
        fd = DeviceFdM;
        signal(SIGIO, sigio_signal_func);
        fcntl(DeviceFdM, F_SETOWN, getpid());		/* 设置当前进程接收SIGIO信号 	*/
        flags = fcntl(DeviceFdM, F_GETFL);			/* 获取当前的进程状态 			*/
        fcntl(DeviceFdM, F_SETFL, flags | FASYNC);	/* 设置进程启用异步通知功能 	*/	
    }

    return true;
}
