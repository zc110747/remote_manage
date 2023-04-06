//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      InternalProcess.cpp
//
//  Purpose:
//      支持node转换接口的服务器处理
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

internal_process*  internal_process::pInstance = nullptr;
internal_process* internal_process::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) internal_process();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "internal_process new error!");
        }
    }
    return pInstance;
}

static AsioServer InterServer;

void internal_process::run()
{
    const SocketSysConfig *pSocketConfig = system_config::getInstance()->getnode();

    try
    {
        InterServer.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [this](char* ptr, int length){
            if(internal_processCmd.parseData(ptr, length))
            {
                //用于处理命令，告知应用
                internal_processCmd.ProcessData();

                //用于处理应答信息
                ProcessCallback();
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
    node_thread = std::thread(std::bind(&internal_process::run, this));
    node_thread.detach();

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

//!status led=ON;BEEP=OFF;IR=1;ALS=1;PS=1;gypox=0;gypoz=0;gypoz=0
void internal_process::SendStatusBuffer(NAMESPACE_DEVICE::device_read_info &info)
{
    char buffer[1024] = {0};
    int CurrentIndex = 0;
    int size;

    memset(buffer, 0, sizeof(buffer));
    size = sprintf(&buffer[0], "!status ");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "LED=%s;", info.led_io==0?"OFF":"ON");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "BEEP=%s;", info.beep_io==0?"OFF":"ON");
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "IR=%d;", info.ap_info.ir);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "ALS=%d;", info.ap_info.als);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "PS=%d;", info.ap_info.ps);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypox=%.2f;", info.icm_info.gyro_x_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypoy=%.2f;", info.icm_info.gyro_y_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "gypoz=%.2f;", info.icm_info.gyro_z_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accelx=%.2f;", info.icm_info.accel_x_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accely=%.2f;", info.icm_info.accel_y_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "accelz=%.2f;", info.icm_info.accel_z_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "temp=%.2f;", info.icm_info.temp_act);
    CurrentIndex += size;
    size = sprintf(&buffer[CurrentIndex], "angle=%d", info.angle);
    CurrentIndex += size;
    buffer[CurrentIndex] = '\0';

    //send '\0' for string finished
    send(buffer, CurrentIndex+1);
}


void internal_process::ProcessCallback()
{
    switch(internal_processCmd.getCurrentFormat())
    {
        case CmdSetDev:
            auto info = NAMESPACE_DEVICE::device_manage::getInstance()->getDeviceInfo();
            SendStatusBuffer(info);
            break;
    }
}