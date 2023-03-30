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

#include "InternalProcess.hpp"
#include "asio_server.hpp"
#include "modules.hpp"

InterProcess*  InterProcess::pInstance = nullptr;
InterProcess* InterProcess::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) InterProcess();
        if(pInstance == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "InterProcess new error!");
        }
    }
    return pInstance;
}

static AsioServer node_server;

void InterProcess::run()
{
    const SocketSysConfig *pSocketConfig = SystemConfig::getInstance()->getnode();

    try
    {
        node_server.init(pSocketConfig->ipaddr, std::to_string(pSocketConfig->port), [this](char* ptr, int length){
            if(InterProcessCmd.parseData(ptr, length))
            {
                //用于处理命令，告知应用
                InterProcessCmd.ProcessData();

                //用于处理应答信息
                ProcessCallback();
            }
        });
        node_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "Exception:%s", e.what());
    }
}

bool InterProcess::init()
{
    node_thread = std::thread(std::bind(&InterProcess::run, this));
    node_thread.detach();

    return true;
}

bool InterProcess::send(char *pbuffer, int size)
{
    bool ret = false;
    
    auto session_list =  node_server.get_session_list();

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
void InterProcess::SendStatusBuffer(NAMESPACE_DEVICE::DeviceReadInfo &info)
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


void InterProcess::ProcessCallback()
{
    switch(InterProcessCmd.getCurrentFormat())
    {
        case CmdSetDev:
            auto info = NAMESPACE_DEVICE::DeviceManageThread::getInstance()->getDeviceInfo();
            SendStatusBuffer(info);
            break;
    }
}