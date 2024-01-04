//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      用于logger打印的模块，支持包含两部分实现。
//      1.用于远程连接的web服务器。
//      2.提供其它模块访问的fifo接口。
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
#include "log_server.hpp"
#include "log_process.hpp"

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

int main(int argc, char *argv[])
{
    bool result = true;

    option_process(argc, argv);

    auto val = daemon(1, 1);
    if (val < 0)
    {
        PRINT_NOW("%s:daemon error!", PRINT_NOW_HEAD_STR);
        return val;
    }

    result = system_init(nConfigDefault, sConfigFile.c_str());

    //only result protect the sem
    if (result)
    {
        for (;;)
        {
            if (global_exit_sem.wait())
            {
                break;
            }
        }
    }

    PRINT_NOW("%s:run stop, error:%s\n", PRINT_NOW_HEAD_STR, strerror(errno));
    return result;
}

static bool system_init(int is_default, const char* path)
{
    bool ret = true;

    //config file manage
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
    }

    ret &= time_manage::get_instance()->init();
    ret &= log_process::get_instance()->init();
    ret &= log_manage::get_instance()->init();//在log_process后执行，需要等待fifo创建
    ret &= log_server::get_instance()->init();

    //std::cout<<*(system_config::get_instance())<<std::endl;
    return ret;
}

void exit_main_app()
{
    global_exit_sem.signal();
}
