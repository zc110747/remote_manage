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

#if MODULE_DEFINE_MQTT == 1
#include "mqtt_process.hpp"
#endif

//internal data
static int nConfigDefault = 0;
static std::string sConfigFile(DEFAULT_CONFIG_FILE);
static EVENT::semaphore global_exit_sem(0);
static option long_opts[] = {
    {"lt_ver", no_argument, NULL, 'v'},
    {"lt_default", no_argument, NULL, 'd'},
    {"lt_file", required_argument, NULL, 'f'},
    {"lt_help", required_argument, NULL, 'h'},
};

//internal function
static bool system_init(int is_default, const char* path);

//用于处理命令行输入的函数
static void option_process(int argc, char *argv[])
{
    int c;

    //命令行输入说明
    //if no parameter_ follow, no ":"
    //must one paramter follow, one ":"
    //can be no or one parameter_, without space
    while ((c = getopt_long(argc, argv, "vdf:h", long_opts, NULL)) != -1)
    {
        switch (c)
        {
            case 'd':
                nConfigDefault = 1;
            break;
            case 'f':
                if (optarg != nullptr)
                {
                    sConfigFile = std::string(optarg);
                    PRINT_NOW("%s:set file:%s!\n", PRINT_NOW_HEAD_STR, sConfigFile.c_str());
                }
            break;
            case '?':
            case 'h':
                printf("Usage: app [options] [file]\n");
                printf("-v       显示版本信息\n");
                printf("-d       使用默认配置\n");
                printf("-h       显示帮组选项\n");
                printf("-f [file]指定选择的配置文件\n");
                exit(0);
            break;
            case 'v':
            case 'V':
            {
                const auto& version = system_config::get_instance()->get_version();
                std::cout<<"Firmware Version:"<<version<<std::endl;
                exit(0);
            }
            default:
            exit(1);
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    option_process(argc, argv);

    //守护进程，用于进程后台执行
    auto result = daemon(1, 1);
    if (result < 0)
    {
        PRINT_NOW("%s:daemon error!\n", PRINT_NOW_HEAD_STR);
        return result;
    }

    system_init(nConfigDefault, sConfigFile.c_str());

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
            return false;
        }
    }
    else
    {
        system_config::get_instance()->default_init();
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "System Config use default!");
    }
    //std::cout<<*(system_config::get_instance())<<std::endl;

    ret &= log_manage::get_instance()->init();
    ret &= time_manage::get_instance()->init();

    #if MODULE_DEFINE_MQTT == 1
    ret &= mqtt_manage::get_instance()->init();
    #endif
    ret &= device_process::get_instance()->init();
    ret &= tcp_thread_manage::get_instance()->init();
    ret &= cmd_process::get_instance()->init();

    return ret;
}

void exit_main_app()
{
    global_exit_sem.signal();
}
