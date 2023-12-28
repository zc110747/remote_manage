//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_process.cpp
//
//  Purpose:
//      local device process.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "device_process.hpp"
#include "common_unit.hpp"

#if MODULE_DEFINE_MQTT == 1
#include "mqtt_process.hpp"
#endif

device_process* device_process::instance_pointer_ = nullptr;
device_process* device_process::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) device_process();
        if (instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "device_process new error!");
        }
    }
    return instance_pointer_;
}

bool device_process::init()
{
    device_info_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_INFO_FIFO, 
                                                            S_FIFO_WORK_MODE, 
                                                            FIFO_MODE_R);
    if (device_info_fifo_point_ == nullptr)
    {
        PRINT_NOW("%s:fifo %s new failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_INFO_FIFO);
        return false;
    }
    if (!device_info_fifo_point_->create())
    {
        PRINT_NOW("%s:fifo %s create failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_INFO_FIFO);
        return false;
    }

    device_cmd_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_CMD_FIFO, 
                                                            S_FIFO_WORK_MODE, 
                                                            FIFO_MODE_W);
    if (device_cmd_fifo_point_ == nullptr)
    {
        PRINT_NOW("%s:fifo %s new failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_CMD_FIFO);
        return false;
    }
    if (!device_cmd_fifo_point_->create())
    {
        PRINT_NOW("%s:fifo %s create failed\r\n", PRINT_NOW_HEAD_STR, LOCAL_DEVICE_CMD_FIFO);
        return false;
    }

    //clear thread
    std::thread(std::bind(&device_process::run, this)).detach();
    return true;
}

void device_process::run()
{
    int size;

    while (1)
    {
        size = device_info_fifo_point_->read(rx_buffer_, DEVICE_RX_BUFFER_SIZE);
        if (size > 0)
        {
            //update rx buffer to internal
            info_.copy_to_device(rx_buffer_);

            //buffer to string
            update_info_string();

            #if MODULE_DEFINE_MQTT == 1
            mqtt_manage::get_instance()->mqtt_publish(info_str_);
            #endif
        }
        else
        {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device rx failed:%d", size);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}

int device_process::send_buffer(const char* ptr, int size)
{
    return device_cmd_fifo_point_->write(ptr, size);
}

void device_process::update_info_string()
{
    Json::Value root;

    root["led"] = info_.led_io_;
    root["beep"] = info_.beep_io_;

    root["ap"]["ir"] = info_.ap_info_.ir;
    root["ap"]["als"] = info_.ap_info_.als;
    root["ap"]["ps"] = info_.ap_info_.ps;   

    root["icm"]["gyro_x"] = info_.icm_info_.gyro_x_act;  
    root["icm"]["gyro_y"] = info_.icm_info_.gyro_y_act;   
    root["icm"]["gyro_z"] = info_.icm_info_.gyro_z_act;  
    root["icm"]["accel_x"] = info_.icm_info_.accel_x_act;  
    root["icm"]["accel_y"] = info_.icm_info_.accel_y_act;   
    root["icm"]["accel_z"] = info_.icm_info_.accel_z_act;    
    root["icm"]["temp_act"] = info_.icm_info_.temp_act; 

    root["angle"] = info_.angle_; 

    info_str_ = root.toStyledString(); 
}

