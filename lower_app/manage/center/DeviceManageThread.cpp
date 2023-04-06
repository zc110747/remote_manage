//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      DeviceManageThread.cpp
//
//  Purpose:
//   	进行设备的管理，周期性的读取硬件信息, 并能够处理外部事件来控制硬件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
#include "DeviceManageThread.hpp"
#include "driver.hpp"
#include "TimeManage.hpp"
#include "CenterManage.hpp"

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
        
        pDevFIFO = new(std::nothrow) FIFOManage(DEVICE_MESSAGE_FIFO, S_FIFO_WORK_MODE);
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
        auto led_ptr = DriverManage::getInstance()->getLed0();
        if(led_ptr->readIoStatus())
        {
            inter_info.led_io = led_ptr->getIoStatus();
        }

        auto beep_ptr = DriverManage::getInstance()->getBeep0();
        if(beep_ptr->readIoStatus())
        {
            inter_info.beep_io = beep_ptr->getIoStatus();
        }

        auto ap_dev_ptr = DriverManage::getInstance()->getApDev0();
        if(ap_dev_ptr->readInfo())
        {
            inter_info.ap_info = ap_dev_ptr->getInfo();
        }

        auto icm_dev_ptr = DriverManage::getInstance()->getIcmDev0();
        if(icm_dev_ptr->readInfo())
        {
            icm_dev_ptr->ConvertInfo();
            inter_info.icm_info = icm_dev_ptr->getConvertInfo();
            inter_info.angle = icm_dev_ptr->getAngle();
            //PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Angle:%d!", icm_dev_ptr->getAngle());
        }

        if(inter_info != outer_info)
        {
            {
                std::lock_guard lock{mut};
                outer_info = inter_info;
            }
            CenterManage::getInstance()->sendInternalHwRefresh();
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
        case EVENT_DEVICE_LED:
            {
                auto led_ptr = DriverManage::getInstance()->getLed0();
                led_ptr->writeIoStatus(action);
            }
            break;
        case EVENT_DEVICE_BEEP:
            {
                auto beep_ptr=DriverManage::getInstance()->getBeep0();
                beep_ptr->writeIoStatus(action);
            }
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
        
        //register action for key process
        DriverManage::getInstance()->getKey0()->register_func([this](int fd){
            unsigned int keyvalue = 0;
            static uint8_t status = 0;

            if(::read(fd, &keyvalue, sizeof(keyvalue)) >= 0)
            {
                if(keyvalue == 1)
                {
                    sendHardProcessMsg(EVENT_DEVICE_LED, status);
                    status = status==0?1:0;
                }
            }
        });

        for(;;)
        {
            size = pDevFIFO->read(buffer, READ_BUFFER_SIZE);
            if(size > 0)
            {
                PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Device Command, %d!", size);
                EventProcess(reinterpret_cast<Event *>(buffer));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    }
}