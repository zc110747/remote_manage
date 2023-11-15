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
#include "tcp_thread.hpp"
#include "uart_thread.hpp"

center_manage* center_manage::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) center_manage;
        if(instance_pointer_ == nullptr)
        {
             PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "device_manage new error!");
        }
    }
    return instance_pointer_;
}

int center_manage::send_message(Event *pMsg, uint16_t size)
{
    return center_fifo_point_->write(reinterpret_cast<char *>(pMsg), size);
}

int center_manage::send_message(uint16_t event_id, uint8_t *msg_pointer, uint16_t size)
{
    uint16_t send_size = size + sizeof(Event);

    char *event_msg = new(std::nothrow) char[send_size];
    if(event_msg == nullptr)
        return 0;

    ((Event *)(event_msg))->set_id(event_id);
    memcpy(event_msg+sizeof(Event), msg_pointer, size);

    send_size = center_fifo_point_->write(event_msg, send_size);
    delete event_msg;

    return send_size;
}

int center_manage::send_hardware_update_message()
{
    Event event(WORKFLOW_ID_HARDWARE_UPDATE);
    return send_message(&event, sizeof(event));
}

int center_manage::send_hardware_config_message(uint8_t device, uint8_t action)
{
    uint8_t buffer[2];

    buffer[0] = device;
    buffer[1] = action;
    return send_message(WORKFLOW_ID_HARDWARE_CHANGE, buffer, 2);
}

void center_manage::send_remote_device_status(const device_read_info &info)
{
    char buffer[128];
    uint16_t size = 0;

    buffer[size++] = TO_REMOTE_UPDATE_STATUS>>8;
    buffer[size++] = TO_REMOTE_UPDATE_STATUS&0xff;

    size += info.copy_to_buffer(&buffer[size]);
    
    tcp_thread_manage::get_instance()->send_msg(buffer, size);
    uart_thread_manage::get_instance()->send_msg(buffer, size);
}

bool center_manage::process_event(Event *pEvent)
{
    uint16_t id = pEvent->get_id();
    uint8_t *pbuffer = (uint8_t *)pEvent;

    switch(id)
    {
    case WORKFLOW_ID_HARDWARE_UPDATE:
        {
            const auto &info = device_manage::get_instance()->get_device_info();

            send_remote_device_status(info);
            internal_process::get_instance()->update_device_status(info);
        }
        break;

    case WORKFLOW_ID_HARDWARE_CHANGE:
        {
            auto *pdata = &pbuffer[sizeof(Event)];
            device_manage::get_instance()->send_device_message(pdata[0], pdata[1]);
            
            // char buffer[] = {0x01, 0x20, 0x30};
            // tcp_thread_manage::get_instance()->send_msg(buffer, 3);
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
    
    center_fifo_point_ = std::make_unique<fifo_manage>(CENTER_UNIT_FIFO, S_FIFO_WORK_MODE);
    if(!center_fifo_point_->create())
    {
        return false;
    }
    
    return true;
}