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

#define FMT_HEADER_ONLY
#include "fmt/core.h"

typedef enum
{
    CMD_NULL = 0,
    CMD_GUI_DEV,
    CMD_LOCAL_DEV,
    CMD_LOWER_DEV,
    CMD_MAIN_DEV,
    CMD_PASSWD,
    CMD_HELP,
}CMD_DEVICE;

#define RX_MAX_BUFFER_SIZE  512
static char rx_buffer[RX_MAX_BUFFER_SIZE];
static const std::map<std::string, CMD_DEVICE> command_map = {
    {"!gui ",           CMD_GUI_DEV},
    {"!local_dev ",     CMD_LOCAL_DEV},
    {"!lower_dev ",     CMD_LOWER_DEV},
    {"!main_proc ",     CMD_MAIN_DEV},
    {"!passwd ",         CMD_PASSWD},
    {"!?",              CMD_HELP},
    {"!help",           CMD_HELP},
};

static const std::map<CMD_DEVICE, std::string> command_help_map = {
    {CMD_GUI_DEV, "!gui [command]:gui cmd"},
    {CMD_LOCAL_DEV, "!local_dev [command]: local device cmd"},
    {CMD_LOWER_DEV, "!lower_dev [command]: lower device cmd"},
    {CMD_MAIN_DEV, "!main_process [command]: main process cmd"},
    {CMD_HELP, "!? or !help"},
};

void log_process::logger_rx_run()
{
    int len;
    PRINT_NOW("%s:logger rx run start!\n", PRINT_NOW_HEAD_STR);

    while (1)
    {
        len = logger_rx_fifo_->read(rx_buffer, RX_MAX_BUFFER_SIZE);
        if (len > 0) {
            log_server::get_instance()->send_buffer(rx_buffer, len);
        } else if (len == 0) {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s read empty fifo data:%d\n", __func__, len);
        } else {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "%s read failed:%d\n", __func__, len);
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
                                                    FIFO_MODE_WR_CREATE);
    if (logger_rx_fifo_ == nullptr)
        return false;
    if (!logger_rx_fifo_->create())
        return false;

    //gui tx fifo
    logger_gui_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_GUI_TX_FIFO,
                                                    S_FIFO_WORK_MODE,
                                                    FIFO_MODE_W_CREATE);
    if (logger_gui_tx_fifo_ == nullptr)
        return false;
    if (!logger_gui_tx_fifo_->create())
        return false;

    //local device tx fifo
    logger_locd_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_LOC_DEV_TX_FIFO,
                                                    S_FIFO_WORK_MODE,
                                                    FIFO_MODE_W_CREATE);
    if (logger_locd_tx_fifo_ == nullptr)
        return false;
    if (!logger_locd_tx_fifo_->create())
        return false;

    //lower device tx fifo
    logger_low_dev_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_LOW_DEV_TX_FIFO,
                                                    S_FIFO_WORK_MODE,
                                                    FIFO_MODE_W_CREATE);
    if (logger_low_dev_tx_fifo_ == nullptr)
        return false;
    if (!logger_low_dev_tx_fifo_->create())
        return false;

    //main process tx_fifo
    logger_mp_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_MP_TX_FIFO,
                                                    S_FIFO_WORK_MODE,
                                                    FIFO_MODE_W_CREATE);
    if (logger_mp_tx_fifo_ == nullptr)
        return false;
    if (!logger_mp_tx_fifo_->create())
        return false;

    logger_rx_thread_ = std::thread(std::bind(&log_process::logger_rx_run, this));
    logger_rx_thread_.detach();

    return true;
}

void log_process::show_help()
{
    std::string out_str = "\n";

    for (const auto [x, y]:command_help_map)
    {
        out_str += fmt::format("{0}\n", y);
    }
    out_str.pop_back();
    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "%s", out_str.c_str());
}

void log_process::release()
{
    logger_rx_fifo_->release();
}

bool log_process::login(char *ptr, int size)
{
    bool allow_no_passwd = system_config::get_instance()->get_allow_no_passwd();
    
    if (allow_no_passwd == true
    || is_login_ == true)
    {
        return true;
    }

    if (strncmp("!passwd ", ptr, strlen("!passwd ")) != 0)
    {
        return false;
    }
    return true;
}

int log_process::send_buffer(char *ptr, int length)
{
    CMD_DEVICE cmd = CMD_NULL;
    int len = 0;
    bool ret;

    ret = login(ptr, length);
    if (!ret)
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "not login:%s", ptr);
        return -1;
    }

    for (const auto& command:command_map)
    {
        if (strncmp (command.first.c_str(), ptr, command.first.length()) == 0)
        {
            cmd = command.second;
            len = command.first.length();
            break;
        }
    }

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "logger command information:%s", ptr);

    if (length >= len)
    {
        switch (cmd)
        {
            case CMD_GUI_DEV:
                logger_gui_tx_fifo_->write(ptr+len, length-len);
                break;
            case CMD_LOCAL_DEV:
                logger_locd_tx_fifo_->write(ptr+len, length-len);
                break;
            case CMD_LOWER_DEV:
                logger_low_dev_tx_fifo_->write(ptr+len, length-len);
                break;
            case CMD_MAIN_DEV:
                logger_mp_tx_fifo_->write(ptr+len, length-len);
                break;
            case CMD_PASSWD:
                {
                    char *pdata;
                    int size;
                    auto passwd = system_config::get_instance()->get_logger_privilege().passwd;

                    pdata = ptr + len;
                    size = length - len;

                    if (size == passwd.length() 
                    && (memcmp(pdata, passwd.c_str(), passwd.length()) == 0)
                    )
                    {
                        is_login_ = true;
                        PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "login in, use !? search support command");
                    }
                    else
                    {
                        PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "passwd:%s, %s, %d, %d", pdata, passwd.c_str(), size, passwd.length());
                    }
                }
                break;
            case CMD_HELP:
                show_help();
                break;
            default:
                PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "no equal command:%s", ptr);
                break;
        }
    }
    else
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "logger command failed:%d, len:%d, %d\n", cmd, length, len);
    }

    return length;
}
