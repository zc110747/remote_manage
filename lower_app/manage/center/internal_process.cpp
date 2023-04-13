//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      InternalProcess.cpp
//
//  Purpose:
//      用于内部通讯的交互，负责和node服务器，GUI显示等外部进程的交互
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/20/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////

#include "internal_process.hpp"
#include "asio_server.hpp"
#include "modules.hpp"

internal_process*  internal_process::instance_pointer_ = nullptr;
internal_process* internal_process::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) internal_process();
        if(instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "internal_process new error!");
        }
    }
    return instance_pointer_;
}

static asio_server InterServer;

void internal_process::run()
{
    const SocketSysConfig *pSocketConfig = system_config::get_instance()->getnode();

    try
    {
        InterServer.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [this](char* ptr, int length){
            if(cmd_process_.parse_data(ptr, length))
            {
                //用于处理命令，告知应用
                cmd_process_.process_data();

                //用于处理应答信息
                process_info_callback();
            }
        });
        InterServer.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Exception:%s", e.what());
    }
}

bool internal_process::init()
{
    node_thread_ = std::thread(std::bind(&internal_process::run, this));
    node_thread_.detach();

    return true;
}

bool internal_process::send(char *pbuffer, int size)
{
    bool ret = false;
    
    auto session_list =  InterServer.get_session_list();

    //数据发送到所有连接的端口
    for(const auto session:session_list)
    {
        if(session != nullptr)
        {
            session->do_write(pbuffer, size);
            ret = true;
        }
    }
    return ret;
}

void internal_process::update_device_status(const NAMESPACE_DEVICE::device_read_info &info)
{
    char buffer[1024] = {0};
    int CurrentIndex = 0;
    int size;

    memset(buffer, 0, sizeof(buffer));
    size = sprintf(&buffer[0], "!status ");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "LED=%s;", info.led_io_==0?"OFF":"ON");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "BEEP=%s;", info.beep_io_==0?"OFF":"ON");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "IR=%d;", info.ap_info_.ir);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "ALS=%d;", info.ap_info_.als);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "PS=%d;", info.ap_info_.ps);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypox=%.2f;", info.icm_info_.gyro_x_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypoy=%.2f;", info.icm_info_.gyro_y_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypoz=%.2f;", info.icm_info_.gyro_z_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accelx=%.2f;", info.icm_info_.accel_x_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accely=%.2f;", info.icm_info_.accel_y_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accelz=%.2f;", info.icm_info_.accel_z_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "temp=%.2f;", info.icm_info_.temp_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "angle=%d", info.angle_);
    CurrentIndex += size;
    buffer[CurrentIndex] = '\0';

    //send '\0' for string finished
    send(buffer, CurrentIndex+1);
}


void internal_process::process_info_callback()
{
    switch(cmd_process_.get_format())
    {
        case CmdSetDev:
            auto info = NAMESPACE_DEVICE::device_manage::get_instance()->get_device_info();
            update_device_status(info);
            break;
    }
}