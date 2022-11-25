//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      WorkflowThread.cpp
//
//  Purpose:
//      Deivce Application Process.
//
// Author:
//     	Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "DeviceManageThread.hpp"
#include "../driver/driver.hpp"
#include "TimeManage.hpp"

DeviceManageThread* DeviceManageThread::pInstance = nullptr;
DeviceManageThread* DeviceManageThread::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) DeviceManageThread();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "DeviceManageThread new error!");
        }
    }
    return pInstance;
}

bool DeviceManageThread::init()
{   
    //init the info
    inter_info.clear();
    outer_info.clear();

    //clear thread
    dmt_thread = std::thread(std::bind(&DeviceManageThread::run, this));
    dmt_thread.detach();
    
    return true;
}

DeviceReadInfo DeviceManageThread::getDeviceInfo()
{
    DeviceReadInfo info;

    {
        std::lock_guard lock{mut};
        info = outer_info;
    }
    
    return info;
}

void DeviceManageThread::update()
{
    if(ledTheOne::getInstance()->readIoStatus())
    {
        inter_info.led_io = ledTheOne::getInstance()->getIoStatus();
    }

    if(beepTheOne::getInstance()->readIoStatus())
    {
        inter_info.beep_io = ledTheOne::getInstance()->getIoStatus();
    }

    if(APDevice::getInstance()->readInfo())
    {
        inter_info.ap_info = APDevice::getInstance()->getInfo();
    }

    if(ICMDevice::getInstance()->readInfo())
    {
        inter_info.icm_info = ICMDevice::getInstance()->getInfo();
    }

    if(inter_info != outer_info)
    {
        std::lock_guard lock{mut};
        outer_info = inter_info;
    }
}

void DeviceManageThread::run()
{
    int flag;
    char buf;
    MessageBase *pMessage;

    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "DeviceManageThread start!");
    pMessage = getMessageInfo(APPLICATION_MESS_INDEX);

    TimeManage::getInstance()->registerWork(0, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](){
        char buf = 1;
        pMessage->write(&buf, sizeof(buf));
    });

    for(;;)
    {
        flag = pMessage->read(&buf, 1);
        if(flag > 0)
        {
            //PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Device Thread Update:%d!", buf);
            update();
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
