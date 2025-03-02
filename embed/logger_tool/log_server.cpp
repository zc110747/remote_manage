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
#include "log_server.hpp"
#include "timer_manage.hpp"
#include "json_config.hpp"
#include "asio_server.hpp"
#include "log_process.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

static asio_server internal_log_server;

//asio server test ok
void log_server::logger_rx_server_run()
{
    const auto& ipaddr = system_config::get_instance()->get_ipaddress();
    const auto& port = system_config::get_instance()->get_logger_port();

    PRINT_NOW("%s:logger server start, ipaddr:%s:%d!\n", PRINT_NOW_HEAD_STR, ipaddr.c_str(), port);

    try
    {
        internal_log_server.init(ipaddr, std::to_string(port), [this](char* ptr, int length){
            log_process::get_instance()->send_buffer(ptr, length);
        });
        internal_log_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_NOW("%s:Exception:%s\n", PRINT_NOW_HEAD_STR, e.what());
    }
}

log_server* log_server::instance_pointer_ = nullptr;
log_server* log_server::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) log_server;
        if (instance_pointer_ == nullptr)
        {
            //do something
        }
    }
    return instance_pointer_;
}

bool log_server::init()
{
    //init thread for logger
    logger_rx_thread_ = std::thread(std::bind(&log_server::logger_rx_server_run, this));
    logger_rx_thread_.detach();

    return true;
}

int log_server::send_buffer(char *pbuffer, uint32_t size)
{
    int len = -1;

    if (internal_log_server.is_valid())
    {
        internal_log_server.do_write(pbuffer, size);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    else
    {
        len = ::write(STDOUT_FILENO, pbuffer, size);
        fflush(stdout);
        if (len < 0)
        {
            //do something
        }
    }

    logger_file_process(pbuffer, size);

    return len;
}

int log_server::logger_file_process(char *buffer, uint32_t size)
{
    time_t timep;
    struct tm mytime, *p;
    std::string filename(LOGS_DIR);

    time(&timep);
    p = localtime_r(&timep, &mytime);
    if(p != NULL)
    {
        filename = fmt::format("/home/sys/logger/{0}-{1}-{2}.txt", mytime.tm_year+1900, mytime.tm_mon+1, mytime.tm_mday);
    }

    if(!outfile.is_open())
    {
        outfile.open(filename, std::ios::app);
    }

    outfile<<std::string(buffer, size);
    outfile.close();

    return 0;
}
