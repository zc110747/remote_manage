//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      center_manage.cpp
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
#include "center_manage.hpp"
#include "logger.hpp"
#include "internal_process.hpp"
#include "device_manage.hpp"
#include <new>

using NAMESPACE_DEVICE::device_manage;
center_manage* center_manage::get_instance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) center_manage;
        if(pInstance == nullptr)
        {
             PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "device_manage new error!");
        }
    }
    return pInstance;
}

int center_manage::send_message(Event *pMsg, uint16_t size)
{
    return center_fifo_point_->write(reinterpret_cast<char *>(pMsg), size);
}

int center_manage::send_hardware_update_message()
{
    Event event(WORKFLOW_ID_HARDWARE_UPDATE);
    return send_message(&event, sizeof(event));
}

int center_manage::send_hardware_config_message(uint8_t device, uint8_t action)
{
    EventBufMessage event(WORKFLOW_ID_HARDWARE_CHANGE);

    event.getData().buffer[0] = device;
    event.getData().buffer[1] = action;
    return send_message(&event, sizeof(event));
}

bool center_manage::process_event(Event *pEvent)
{
    uint16_t id = pEvent->getId();
    switch(id)
    {
    case WORKFLOW_ID_HARDWARE_UPDATE:
        {
            auto info = device_manage::get_instance()->get_device_info();
            internal_process::get_instance()->update_device_status(info);
        }
        break;

    case WORKFLOW_ID_HARDWARE_CHANGE:
        {
            auto *pMessage = static_cast<EventBufMessage *>(pEvent);
            device_manage::get_instance()->send_device_message
                (pMessage->getData().buffer[0], pMessage->getData().buffer[1]);
        }
        break;
    default:
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device Command:%d!", id);
        break;
    }
    return true;
}

void center_manage::run()
{
    char buffer[1024];
    int size;
    
    for(;;)
    {
        size = center_fifo_point_->read(buffer, READ_BUFFER_SIZE);
        if(size > 0)
        {
            PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "center_manage Command, %d!", size);
            process_event(reinterpret_cast<Event *>(buffer));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

bool center_manage::init()
{
    //clear thread
    std::thread(std::bind(&center_manage::run, this)).detach();
    
    center_fifo_point_ = new(std::nothrow) fifo_manage(CENTER_UNIT_FIFO, S_FIFO_WORK_MODE);
    if(center_fifo_point_ == nullptr)
    {
        return false;
    }
    
    return center_fifo_point_->create();
}