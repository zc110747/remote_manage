//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_manage.cpp
//
//  Purpose:
//      command process for local device.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "cmd_process.hpp"
#include "common_unit.hpp"
#include "device_process.hpp"
#include "json/json.h"

const static std::map<std::string, cmd_format_t> CmdMapM = {
    {"getos",           cmdGetOS},
    {"setlevel",        cmdSetLevel},
    {"setdev",          cmdSetDevice},
    {"?",               cmdGetHelp},
    {"help",            cmdGetHelp},
};

const static std::map<cmd_format_t, std::string> CmdHelpMapM = {
    {cmdGetOS,          "!main_proc getos"},
    {cmdSetLevel,       "!main_proc setlevel [app],[lev 0-5]"},
    {cmdSetDevice,      "!main_proc setdev [dev],[data]"},
    {cmdGetHelp,        "!main_proc ? or !mainprocess help"},
};

const static std::map<std::string, int> DevMap = {
    {"led",     DEVICE_LED},
    {"beep",    DEVICE_BEEP},
    {"pwm",     DEVICE_PWM}
};

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream{s};
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

cmd_process* cmd_process::instance_pointer_ = nullptr;
cmd_process* cmd_process::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) cmd_process();
        if (instance_pointer_ == nullptr)
        {
            PRINT_NOW("%s:device_manage new failed\r\n", PRINT_NOW_HEAD_STR);
        }
    }
    return instance_pointer_;
}

bool cmd_process::init()
{
    logger_main_process_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_MP_TX_FIFO, 
                                                    S_FIFO_WORK_MODE, 
                                                    FIFO_MODE_R);
    if (logger_main_process_tx_fifo_ == nullptr)
        return false;
    if (!logger_main_process_tx_fifo_->create())
        return false;

    cmd_process_thread_ = std::thread(std::bind(&cmd_process::run, this));
    cmd_process_thread_.detach();

    return true;
}

bool cmd_process::parse_data()
{
    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "data:%s", rx_buffer_);
    //replace first ' ' by '\0'
    char *pStart = rx_buffer_;
    while ((*pStart != ' ') && (*pStart != '\0'))
        pStart++;
    pStart[0] = '\0';

    //将数据lower,解决数据不符合问题
    auto strVal = std::string(rx_buffer_);
    std::string strDst;
    strDst.resize(strVal.size());
    std::transform(strVal.begin(), strVal.end(), strDst.begin(), ::tolower);
    
    if (CmdMapM.count(strDst) == 0)
    {
        return false;
    }

    cmd_format_ = CmdMapM.find(strDst)->second;
    cmd_data_pointer_ = pStart+1;

    return true;
}

void cmd_process::show_os()
{
    auto pSysConfig = system_config::get_instance();
    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "FW_Version:%s", pSysConfig->get_version().c_str());
    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "Logger Level:%d ", (int)log_manage::get_instance()->get_level());
}

void cmd_process::sync_level(int dev, int level)
{
    int last_level;

    switch (dev)
    {
        case GUI_LOGGER_DEV:
            break;
        case LOCAL_LOGGER_DEV:
            last_level = system_config::get_instance()->get_logger_privilege().local_device_level;
            if (last_level != level)
            {   
                char buf[2];

                buf[0] = 0;
                buf[1] = level;
                device_process::get_instance()->sync_info(buf, 2);
            }
            break;
        case LOGGER_LOGGER_DEV:
            //without interface to process
            break;
        case LOWER_LOGGER_DEV:
            break;
        case MAIN_LOGGER_DEV:
            last_level = system_config::get_instance()->get_logger_privilege().main_process_level;
            if (last_level != level)
            {
                log_manage::get_instance()->set_level((LOG_LEVEL)level);
            }
            break;
        case NODE_LOGGGE_DEV:
            break;
    }

    system_config::get_instance()->set_logger_level(dev, level);
}

//!main_proc setdev pwm;1,50000,25000
bool cmd_process::process_data()
{
    bool ret = true;
    switch (cmd_format_)
    {
        case cmdGetOS:
            show_os();
            break;
            
        case cmdSetLevel:
            {
                int dev, level;
                
                sscanf(cmd_data_pointer_, "%d,%d", &dev, &level);
                PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "dev:%d, level:%d", dev, level);
                sync_level(dev, level);
            }
            break;
        case cmdSetDevice:
            {
                std::string data_str(cmd_data_pointer_);
                auto token = split(data_str, ';');
                int dev;

                if (DevMap.count(token[0].c_str()) == 0)
                {
                    PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid device:%s", token[0].c_str());
                    return false;
                } 
                dev = DevMap.find(token[0].c_str())->second;
                process_device(dev, token[1]);
            }
            break;
        case cmdGetHelp:
            {
                for (auto &[x, y] : CmdHelpMapM)
                {
                    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), y.c_str());
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            }
            break;
    }

    return ret;
}

bool cmd_process::process_device(int dev, const std::string& data)
{
    switch(dev)
    {
        case DEVICE_LED:
        case DEVICE_BEEP:
            {
                int state;
                state = (data == "on"?1:0);  
                device_process::get_instance()->set_device(dev, (char *)&state, 1);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "set device:%d,%d", dev, state);
            }
            break;
        case DEVICE_PWM:
            {
                uint8_t buffer[16], size = 0;
                int state, peroid, duty_cycle;
                sscanf(data.c_str(), "%d;%d;%d", &state, &peroid, &duty_cycle);

                buffer[size++] = state;
                buffer[size++] = peroid>>24;
                buffer[size++] = peroid>>16;
                buffer[size++] = peroid>>8;                               
                buffer[size++] = peroid;
                buffer[size++] = duty_cycle>>24;
                buffer[size++] = duty_cycle>>16;
                buffer[size++] = duty_cycle>>8;                               
                buffer[size++] = duty_cycle;
                device_process::get_instance()->set_device(dev, (char *)buffer, size);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "pwm set:%d,%d,%d", state, peroid, duty_cycle);
            }
            break;
    }

    return true;
}


/*
{"command":"setdev", source:0, "device":"led", "data":"on"}
{"command":"setdev", source:1, "device":"pwm", "data":"1,50000,25000"}
*/
bool cmd_process::mqtt_decode_command(char *ptr, int size)
{
    try
    {
        Json::CharReaderBuilder readerBuilder;
        std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
        Json::Value root;
        char const* begin = ptr;
        char const* end = begin + size;
        JSONCPP_STRING errs;
        std::string data;
        cmd_format_t format;
        int source;

        if(!reader->parse(begin, end, &root, &errs))
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "json reader parse failed");
            return false;
        }

        data = root["command"].asString();
        if (CmdMapM.count(data) == 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid commnd:%s", data.c_str());
            return false;
        } 
        format = CmdMapM.find(data)->second;
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "commnd:%s, index:%d", data.c_str(), format);
        source = root["source"].asInt();
        switch(format)
        {
            case cmdSetDevice:
                {
                    int dev;
                    data = root["device"].asString();
                    if (DevMap.count(data) == 0)
                    {
                        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid device:%s", data.c_str());
                        return false;
                    } 
                    dev = DevMap.find(data)->second;
                    data = root["data"].asString();
                    process_device(dev, data);
                }
                break;
        }

        /* code */
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

void cmd_process::run()
{
    int len;

    while (1)
    {
        len = logger_main_process_tx_fifo_->read(rx_buffer_, DEVICE_RX_BUFFER_SIZE);
        if (len > 0)
        {
            rx_buffer_[len] = '\0';
            rx_size_ = len;

            if (parse_data())
            {
                process_data();
            }
            else
            {
                PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "cmd_process parse fail, buffer:%s!", rx_buffer_);
            }
        }
        else if (len == 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "%s read empty fifo data:%d\n", __func__, len);
        }
        else
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "%s read failed:%d\n", __func__, len);
            break;
        }
    }
}
