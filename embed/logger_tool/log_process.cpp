//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      log_server.cpp
//
//  Purpose:
//      logger printf format:
//      [ticks][process][level]:[info]
//      [00:00:00][logger_tool][0]:[start info]
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "include/productConfig.hpp"
#include "log_process.hpp"
#include "log_server.hpp"

#define RX_MAX_BUFFER_SIZE 1024
static char rx_buffer[RX_MAX_BUFFER_SIZE];

void log_process::logger_rx_run()
{
    int len;

    while (1)
    {
        len = logger_rx_fifo_->read(rx_buffer, RX_MAX_BUFFER_SIZE);
        if (len > 0)
        {
            log_server::get_instance()->send_buffer(rx_buffer, len);
        }
        else if (len == 0)
        {
            PRINT_NOW("%s read empty fifo data:%d\n", __func__, len);
        }
        else
        {
            PRINT_NOW("%s read failed:%d\n", __func__, len);
            break;
        }
    }
}

log_process* log_process::instance_pointer_ = nullptr;
log_process* log_process::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) log_process;
        if (instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

bool log_process::init()
{
    //init and Create logger fifo, must before thread run.
    logger_rx_fifo_ = std::make_unique<fifo_manage>(LOGGER_RX_FIFO, 
                                                    S_FIFO_WORK_MODE, 
                                                    FIFO_MODE_R_CREATE);
    if (logger_rx_fifo_ == nullptr)
        return false;
    if (!logger_rx_fifo_->create())
        return false;

    logger_rx_thread_ = std::thread(std::bind(&log_process::logger_rx_run, this));
    logger_rx_thread_.detach();

    return true;
}

void log_process::release()
{
    logger_rx_fifo_->release();
}
