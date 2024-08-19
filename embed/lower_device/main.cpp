//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//     串口转换为serial_net,
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
//
/////////////////////////////////////////////////////////////////////////////
#include <getopt.h>
#include "common_unit.hpp"
#include "serial.hpp"
#include "asio_client.hpp"
#include "parameter_parser.hpp"

//internal data
static EVENT::semaphore global_exit_sem(0);
struct _Params gParameter;

//internal function
static bool system_init(int is_default, const char* path);

int main(int argc, char* argv[])
{
    parse_parameter(&gParameter, argc, argv);

    //守护进程，用于进程后台执行
    auto result = daemon(1, 1);
    if (result < 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "daemon error!");
        return result;
    }

    system_init(gParameter.is_default_config, gParameter.configfile.c_str());

    for (;;)
    {
        if (global_exit_sem.wait())
        {
            break;
        }
    }

    log_manage::get_instance()->release();

    PRINT_NOW("Process app_demo stop, error:%s\n", strerror(errno));
    return result;
}

//系统模块初始化
//主要包含调试接口，硬件驱动，设备管理
//网络协议通讯，串口协议通讯，以及时间周期触发的循环事件
static bool system_init(int is_default, const char* path)
{
    bool ret = true;

    //选中配置文件
    if (is_default == 0)
    {
        system_config::get_instance()->init(path);
        if (!ret)
        {
            return false;
        }
    }
    else
    {
        system_config::get_instance()->default_init();
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "System Config use default!");
    }
    std::cout<<*(system_config::get_instance())<<std::endl;
    LOG_LEVEL level = (LOG_LEVEL)system_config::get_instance()->get_logger_privilege().lower_device_level;
    log_manage::get_instance()->set_level(level);

    ret &= log_manage::get_instance()->init();
    ret &= time_manage::get_instance()->init();
    ret &= serial_manage::get_instance()->init();
    ret &= asio_client::get_instance()->init();

    return ret;
}

void exit_main_app()
{
    global_exit_sem.signal();
}
