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

#if MODULE_DEFINE_MQTT == 1
#include "mqtt_process.hpp"
#endif

device_manage* device_manage::instance_pointer_ = nullptr;
device_manage* device_manage::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) device_manage();
        if(instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "device_manage new error!");
        }
    }
    return instance_pointer_;
}

bool device_manage::init()
{   
    //init the info
    inter_info_.clear();
    outer_info_.clear();

    //clear thread
    std::thread(std::bind(&device_manage::run, this)).detach();
    
    device_fifo_point_ = std::make_unique<fifo_manage>(DEVICE_MESSAGE_FIFO, S_FIFO_WORK_MODE);
    if(!device_fifo_point_->create())
    {
        return false;
    }
    
    return true;
}

int device_manage::send_device_message(uint8_t device, uint8_t action)
{
    EventBufMessage ebufMsg(DEVICE_ID_HARDWARE_CHANGE);

    ebufMsg.get_data().buffer[0] = device;
    ebufMsg.get_data().buffer[1] = action;

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
        std::lock_guard lock{mutex_};
        info = outer_info_;
    }
    
    return info;
}

void device_manage::update()
{
    auto led_ptr = driver_manage::get_instance()->get_led_zero();
    if(led_ptr->read_io_status())
    {
        inter_info_.led_io_ = led_ptr->get_io_status();
    }

    auto beep_ptr = driver_manage::get_instance()->get_beep_zero();
    if(beep_ptr->read_io_status())
    {
        inter_info_.beep_io_ = beep_ptr->get_io_status();
    }

    auto ap_dev_ptr = driver_manage::get_instance()->get_ap3126_dev();
    if(ap_dev_ptr->read_info())
    {
        inter_info_.ap_info_ = ap_dev_ptr->get_info();
    }

    auto icm_dev_ptr = driver_manage::get_instance()->get_icm20608_dev();
    if(icm_dev_ptr->read_info())
    {
        icm_dev_ptr->calculate_angle();
        inter_info_.icm_info_ = icm_dev_ptr->get_icm_info();
        inter_info_.angle_ = icm_dev_ptr->get_angle();
        //PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Angle:%d!", icm_dev_ptr->get_angle());
    }

    if(inter_info_ != outer_info_)
    {
        {
            std::lock_guard lock{mutex_};
            outer_info_ = inter_info_;
        }
        center_manage::get_instance()->send_hardware_update_message();
    }

    //mqtt publish
    #if MODULE_DEFINE_MQTT == 1
        update_device_string();
        mqtt_publish(outer_str_);
    #endif
}

void device_manage::update_device_string()
{
    Json::Value root;

    root["led"] = outer_info_.led_io_;
    root["beep"] = outer_info_.beep_io_;

    root["ap"]["ir"] = outer_info_.ap_info_.ir;
    root["ap"]["als"] = outer_info_.ap_info_.als;
    root["ap"]["ps"] = outer_info_.ap_info_.ps;   

    root["icm"]["gyro_x"] = outer_info_.icm_info_.gyro_x_act;  
    root["icm"]["gyro_y"] = outer_info_.icm_info_.gyro_y_act;   
    root["icm"]["gyro_z"] = outer_info_.icm_info_.gyro_z_act;  
    root["icm"]["accel_x"] = outer_info_.icm_info_.accel_x_act;  
    root["icm"]["accel_y"] = outer_info_.icm_info_.accel_y_act;   
    root["icm"]["accel_z"] = outer_info_.icm_info_.accel_z_act;    
    root["icm"]["temp_act"] = outer_info_.icm_info_.temp_act; 

    root["angle"] = outer_info_.angle_; 

    outer_str_ = root.toStyledString(); 
}

void device_manage::process_hardware(Event *pEvent)
{
    EventBufMessage *pHardEvent = static_cast<EventBufMessage *>(pEvent);
    auto data = pHardEvent->get_data();

    uint8_t device = data.buffer[0];
    uint8_t action = data.buffer[1];

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Device Process:%d, %d!", device, action);

    switch (device)
    {
    case EVENT_DEVICE_LED:
        {
            auto led_ptr = driver_manage::get_instance()->get_led_zero();
            led_ptr->write_io_status(action);
        }
        break;
    case EVENT_DEVICE_BEEP:
        {
            auto beep_ptr=driver_manage::get_instance()->get_beep_zero();
            beep_ptr->write_io_status(action);
        }
        break;
    default:
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Device:%d!", device);
        break;
    }
}

bool device_manage::process_event(Event *pEvent)
{
    uint16_t id = pEvent->get_id();
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

    time_manage::get_instance()->register_action(DEVICE_ID_TIME_UPDATE_PREOID, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](int id){
        Event event(id);
        send_message(reinterpret_cast<char *>(&event), sizeof(event));
    });
    
    //register action for key process
    driver_manage::get_instance()->get_key_zero()->register_func([this](int fd){
        unsigned int keyvalue = 0;
        static uint8_t status = 0;

        if(::read(fd, &keyvalue, sizeof(keyvalue)) >= 0)
        {
            if(keyvalue == 1)
            {
                status = status==0?1:0;
                send_device_message(EVENT_DEVICE_LED, status);
            }
        }
    });

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "device_manage start!");
    
    for(;;)
    {
        size = device_fifo_point_->read(buffer, READ_BUFFER_SIZE);
        if(size > 0)
        {
            PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Device Command, size:%d, id:%d!", 
                size,
                reinterpret_cast<Event *>(buffer)->get_id());
            process_event(reinterpret_cast<Event *>(buffer));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}