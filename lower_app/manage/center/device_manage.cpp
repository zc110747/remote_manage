//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      device_manage.cpp
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
#include "device_manage.hpp"
#include "driver.hpp"
#include "time_manage.hpp"
#include "center_manage.hpp"

namespace NAMESPACE_DEVICE
{
    device_manage* device_manage::pInstance = nullptr;
    device_manage* device_manage::getInstance()
    {
        if(pInstance == nullptr)
        {
            pInstance = new(std::nothrow) device_manage();
            if(pInstance == nullptr)
            {
                PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "device_manage new error!");
            }
        }
        return pInstance;
    }

    bool device_manage::init()
    {   
        //init the info
        inter_info_.clear();
        outer_info_.clear();

        //clear thread
        std::thread(std::bind(&device_manage::run, this)).detach();
        
        device_fifo_point_ = new(std::nothrow) fifo_manage(DEVICE_MESSAGE_FIFO, S_FIFO_WORK_MODE);
        if(device_fifo_point_ == nullptr)
        {
            return false;
        }
        
        return device_fifo_point_->create();
    }

    int device_manage::send_device_message(uint8_t device, uint8_t action)
    {
        EventBufMessage ebufMsg(DEVICE_ID_HARDWARE_CHANGE);

        ebufMsg.getData().buffer[0] = device;
        ebufMsg.getData().buffer[1] = action;

        return send_message(reinterpret_cast<char *>(&ebufMsg), sizeof(ebufMsg));
    }

    int device_manage::send_message(char* pEvent, int size)
    {
        return device_fifo_point_->write(pEvent, size);
    }
    
    device_read_info device_manage::get_device_info()
    {
        device_read_info info;

        {
            std::lock_guard lock{mut_};
            info = outer_info_;
        }
        
        return info;
    }

    void device_manage::update()
    {
        auto led_ptr = driver_manage::getInstance()->getLed0();
        if(led_ptr->readIoStatus())
        {
            inter_info_.led_io_ = led_ptr->getIoStatus();
        }

        auto beep_ptr = driver_manage::getInstance()->getBeep0();
        if(beep_ptr->readIoStatus())
        {
            inter_info_.beep_io_ = beep_ptr->getIoStatus();
        }

        auto ap_dev_ptr = driver_manage::getInstance()->get_ap3126_dev();
        if(ap_dev_ptr->readInfo())
        {
            inter_info_.ap_info_ = ap_dev_ptr->getInfo();
        }

        auto icm_dev_ptr = driver_manage::getInstance()->getIcmDev0();
        if(icm_dev_ptr->readInfo())
        {
            icm_dev_ptr->ConvertInfo();
            inter_info_.icm_info_ = icm_dev_ptr->getConvertInfo();
            inter_info_.angle_ = icm_dev_ptr->getAngle();
            //PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Angle:%d!", icm_dev_ptr->getAngle());
        }

        if(inter_info_ != outer_info_)
        {
            {
                std::lock_guard lock{mut_};
                outer_info_ = inter_info_;
            }
            center_manage::getInstance()->send_hardware_update_message();
        }
    }

    void device_manage::process_hardware(Event *pEvent)
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
                auto led_ptr = driver_manage::getInstance()->getLed0();
                led_ptr->writeIoStatus(action);
            }
            break;
        case EVENT_DEVICE_BEEP:
            {
                auto beep_ptr=driver_manage::getInstance()->getBeep0();
                beep_ptr->writeIoStatus(action);
            }
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device:%d!", device);
            break;
        }
    }

    bool device_manage::process_event(Event *pEvent)
    {
        uint16_t id = pEvent->getId();
        switch(id)
        {
        case DEVICE_ID_TIME_UPDATE_PREOID:
            update();
            break;
        case DEVICE_ID_HARDWARE_CHANGE:
            process_hardware(pEvent);
            break;
        default:
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device Command:%d!", id);
            break;
        }
        return true;
    }

    void device_manage::run()
    {
        int size;
        char buffer[READ_BUFFER_SIZE];

        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "device_manage start!");
        time_manage::getInstance()->registerWork(0, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](){
            Event event(DEVICE_ID_TIME_UPDATE_PREOID);
            send_message(reinterpret_cast<char *>(&event), sizeof(event));
        });
        
        //register action for key process
        driver_manage::getInstance()->getKey0()->register_func([this](int fd){
            unsigned int keyvalue = 0;
            static uint8_t status = 0;

            if(::read(fd, &keyvalue, sizeof(keyvalue)) >= 0)
            {
                if(keyvalue == 1)
                {
                    send_device_message(EVENT_DEVICE_LED, status);
                    status = status==0?1:0;
                }
            }
        });

        for(;;)
        {
            size = device_fifo_point_->read(buffer, READ_BUFFER_SIZE);
            if(size > 0)
            {
                PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Device Command, %d!", size);
                process_event(reinterpret_cast<Event *>(buffer));
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
    }
}