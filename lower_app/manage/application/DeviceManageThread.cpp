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
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "DeviceManageThread new error!");
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
    
    pDevFIFO = new(std::nothrow) FIFOMessage(DEVICE_MESSAGE_FIFO, S_FIFO_WORK_MODE);
    if(pDevFIFO == nullptr)
    {
        return false;
    }
    
    return pDevFIFO->Create();
}

int DeviceManageThread::sendMessage(Event* pEvent)
{
    int size;
    size = pDevFIFO->write(reinterpret_cast<char*>(pEvent), pEvent->size());
    return size;
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

bool DeviceManageThread::EventProcess(Event *pEvent)
{
    switch(pEvent->getId())
    {
    case DEVICE_ID_TIME_UPDATE_PREOID:
        update();
        break;
    default:
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device Command!");
        break;
    }
    return true;
}

void DeviceManageThread::run()
{
    int size;
    char buffer[READ_BUFFER_SIZE];

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "DeviceManageThread start!");
    TimeManage::getInstance()->registerWork(0, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](){
        Event event(DEVICE_ID_TIME_UPDATE_PREOID);
        sendMessage(&event);
    });

    for(;;)
    {
        size = pDevFIFO->read(buffer, READ_BUFFER_SIZE);
        if(size > 0)
        {
            //PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Device Command, %d!", size);
            EventProcess(reinterpret_cast<Event *>(buffer));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
