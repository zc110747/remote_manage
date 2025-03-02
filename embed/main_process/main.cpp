//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      本项目作为嵌入式管理平台的下位机，主要负责网络，串口通讯，本地设备管理，
//远端设备轮询。
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
#include "device_process.hpp"
#include "tcp_thread.hpp"
#include "cmd_process.hpp"
#include "parameter_parser.hpp"
#include "mb_app.hpp"

#if MODULE_DEFINE_MQTT == 1
#include "mqtt_process.hpp"
#endif

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
        PRINT_NOW("%s:daemon error!\n", PRINT_NOW_HEAD_STR);
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

    PRINT_NOW("%s:process main_process stop, error:%s\n", PRINT_NOW_HEAD_STR, strerror(errno));
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
        ret = system_config::get_instance()->init(path);
        if (!ret)
        {
            PRINT_NOW("%s:system_config init failed!\n", PRINT_NOW_HEAD_STR);
            return false;
        }
    }
    else
    {
        system_config::get_instance()->default_init();
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "System Config use default!");
    }
    std::cout<<*(system_config::get_instance())<<std::endl;
    LOG_LEVEL level = (LOG_LEVEL)system_config::get_instance()->get_logger_privilege().main_process_level;
    log_manage::get_instance()->set_level(level);
    
    ret &= log_manage::get_instance()->init();
    ret &= timer_manage::get_instance()->init();

    #if MODULE_DEFINE_MQTT == 1
    ret &= mqtt_manage::get_instance()->init();
    #endif
    ret &= device_process::get_instance()->init();
    ret &= tcp_thread_manage::get_instance()->init();
    ret &= cmd_process::get_instance()->init();
    ret &= mb_app_manage::get_instance()->init();

    return ret;
}

void exit_main_app()
{
    global_exit_sem.signal();
}
