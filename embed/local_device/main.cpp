//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      main.cpp
//
//  Purpose:
//      local device manage.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/21/2023   Create New Version
//
/////////////////////////////////////////////////////////////////////////////
#include <getopt.h>
#include "common_unit.hpp"
#include "device_manage.hpp"
#include "module/cmd_process.hpp"
#include "parameter_parser.hpp"

// internal data
static EVENT::semaphore global_exit_sem(0);
struct _Params gParameter;

// internal function
static bool system_init(int is_default, const char *path);

int main(int argc, char *argv[])
{
    bool result = true;

    parse_parameter(&gParameter, argc, argv);

    // 守护进程，用于进程后台执行
    auto val = daemon(1, 1);
    if (val < 0)
    {
        PRINT_NOW("%s:daemon error!", PRINT_NOW_HEAD_STR);
        return val;
    }

    system_init(gParameter.is_default_config, gParameter.configfile.c_str());

    // only result protect the sem
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

static bool system_init(int is_default, const char *path)
{
    bool ret = true;

    // config file manage
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
    }
    
    LOG_LEVEL level = (LOG_LEVEL)system_config::get_instance()->get_logger_privilege().local_device_level;
    log_manage::get_instance()->set_level(level);

    ret &= log_manage::get_instance()->init();
    ret &= time_manage::get_instance()->init();
    ret &= driver_manage::get_instance()->init();
    ret &= device_manage::get_instance()->init();
    ret &= cmd_process::get_instance()->init();

    return ret;
}
