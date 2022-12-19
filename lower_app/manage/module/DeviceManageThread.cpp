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
#include "driver.hpp"
#include "TimeManage.hpp"

namespace NAMESPACE_DEVICE
{
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
        std::thread(std::bind(&DeviceManageThread::run, this)).detach();
        
        pDevFIFO = new(std::nothrow) FIFOMessage(DEVICE_MESSAGE_FIFO, S_FIFO_WORK_MODE);
        if(pDevFIFO == nullptr)
        {
            return false;
        }
        
        return pDevFIFO->Create();
    }

    int DeviceManageThread::sendHardProcessMsg(uint8_t device, uint8_t action)
    {
        EventBufMessage ebufMsg(DEVICE_ID_HARDWARE_CHANGE);

        ebufMsg.getData().buffer[0] = device;
        ebufMsg.getData().buffer[1] = action;

        return sendMessage(reinterpret_cast<char *>(&ebufMsg), sizeof(ebufMsg));
    }

    int DeviceManageThread::sendMessage(char* pEvent, int size)
    {
        return pDevFIFO->write(pEvent, size);
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

    void DeviceManageThread::HardProcess(Event *pEvent)
    {
        EventBufMessage *pHardEvent = static_cast<EventBufMessage *>(pEvent);
        auto data = pHardEvent->getData();

        uint8_t device = data.buffer[0];
        uint8_t action = data.buffer[1];

        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Device Process:%d, %d!", device, action);

        switch (device)
        {
        case EVENT_HADWARE_LED:
            ledTheOne::getInstance()->writeIoStatus(action);
            break;
        case EVENT_HADWARE_BEEP:
            beepTheOne::getInstance()->writeIoStatus(action);
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device:%d!", device);
            break;
        }
    }

    bool DeviceManageThread::EventProcess(Event *pEvent)
    {
        uint16_t id = pEvent->getId();
        switch(id)
        {
        case DEVICE_ID_TIME_UPDATE_PREOID:
            update();
            break;
        case DEVICE_ID_HARDWARE_CHANGE:
            HardProcess(pEvent);
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device Command:%d!", id);
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
            sendMessage(reinterpret_cast<char *>(&event), sizeof(event));
        });

        for(;;)
        {
            size = pDevFIFO->read(buffer, READ_BUFFER_SIZE);
            if(size > 0)
            {
                PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Device Command, %d!", size);
                EventProcess(reinterpret_cast<Event *>(buffer));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    }
}