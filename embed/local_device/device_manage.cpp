
//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_manage.cpp
//
//  Purpose:
//      包含应用配置信息的文件
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
#include "common_unit.hpp"
#include "include/productConfig.hpp"

device_manage* device_manage::instance_pointer_ = nullptr;
device_manage* device_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) device_manage();
        if (instance_pointer_ == nullptr)
        {
            PRINT_NOW("%s:device_manage new failed\r\n", PRINT_NOW_HEAD_STR);
        }
    }
    return instance_pointer_;
}

bool device_manage::init()
{   
    inter_info_.clear();
    outer_info_.clear();

    //command receive fifo
    device_cmd_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_CMD_FIFO, FIFO_MODE_R_CREATE);
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

    //info translate fifo
    device_info_fifo_point_ = std::make_unique<fifo_manage>(LOCAL_DEVICE_INFO_FIFO, FIFO_MODE_W_CREATE);
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

    //start thread
    std::thread(std::bind(&device_manage::run, this)).detach();
    return true;
}

void device_manage::update()
{
    {
        //device hardware protect
        std::lock_guard lock{mutex_};
        auto led_ptr = driver_manage::get_instance()->get_led_zero();
        if (led_ptr->read_io_status())
        {
            inter_info_.led_io_ = led_ptr->get_io_status();
        }

        auto beep_ptr = driver_manage::get_instance()->get_beep_zero();
        if (beep_ptr->read_io_status())
        {
            inter_info_.beep_io_ = beep_ptr->get_io_status();
        }

        auto ap_dev_ptr = driver_manage::get_instance()->get_ap3126_dev();
        if (ap_dev_ptr->read_info())
        {
            inter_info_.ap_info_ = ap_dev_ptr->get_info();
        }

        auto icm_dev_ptr = driver_manage::get_instance()->get_icm20608_dev();
        if (icm_dev_ptr->read_info())
        {
            icm_dev_ptr->calculate_angle();
            inter_info_.icm_info_ = icm_dev_ptr->get_icm_info();
            inter_info_.angle_ = icm_dev_ptr->get_angle();
            //PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Angle:%d!", icm_dev_ptr->get_angle());
        }
    }

    //if (inter_info_ != outer_info_)
    {
        uint8_t size;

        outer_info_ = inter_info_;
        size = outer_info_.copy_to_buffer(tx_buffer);

        device_info_fifo_point_->write(tx_buffer, size);
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "update to main porcess, size:%d!", size);
    }
    // else
    // {
    //     PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device no change!");
    // }
}



void device_manage::run()
{
    int size;
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "device_manage start!");

    time_manage::get_instance()->register_action(DEVICE_LOOP_EVENT, TIME_TICK(1000), TIME_ACTION_ALWAYS, [&](int id){
        //time loop update device.
        this->update();
    });
    
    for (;;)
    {
        size = device_cmd_fifo_point_->read(rx_buffer, RX_BUFFER_SIZE);
        if (size > 0)
        {
            //add command process
        }
        else
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "device_cmd_fifo_point_ rx failed!");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
}
