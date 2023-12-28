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

/*
!readdev    [index] #index=[0~3 led,beep,ap,icm]
!setdev     [index],[data] #index=[0~1 led,beep]  
!getNet     [index] #index=[0~2 udp,tcp,logger] 
!testDev    [index] #index=[0~3 led,beep,ap,icm]      
!getSerial  
!? or !help
*/
const static std::map<std::string, cmd_format_t> CmdMapM = {
    {"readdev",    CmdReadDev},
    {"setdev",     CmdSetDev},
    {"setlevel",   cmdSetLevel},
    {"?",          CmdGetHelp},
    {"help",       CmdGetHelp},
};

const static std::map<cmd_format_t, std::string> CmdHelpMapM = {
    {CmdReadDev,    "!localdevice readdev"},
    {CmdSetDev,     "!localdevice setdev [index],[action]"},
    {cmdSetLevel,   "!localdevice setlevel [lev 0-5]",},
    {CmdGetHelp,    "!localdevice ? or !locd help"},
};

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
    logger_loc_dev_tx_fifo_ = std::make_unique<fifo_manage>(LOGGER_LOC_DEV_TX_FIFO, 
                                                    S_FIFO_WORK_MODE, 
                                                    FIFO_MODE_R);
    if (logger_loc_dev_tx_fifo_ == nullptr)
        return false;
    if (!logger_loc_dev_tx_fifo_->create())
        return false;

    cmd_process_thread_ = std::thread(std::bind(&cmd_process::run, this));
    cmd_process_thread_.detach();

    return true;
}

bool cmd_process::parse_data()
{
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

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "rx command:%s", strDst.c_str());
    if (CmdMapM.count(strDst) == 0)
    {
        return false;
    }

    cmd_format_ = CmdMapM.find(strDst)->second;
    cmd_data_pointer_ = pStart+1;

    return true;
}

bool cmd_process::process_data()
{
    bool ret = true;
    switch (cmd_format_)
    {
        case CmdReadDev:
            break;
        case CmdSetDev:
            break;
        case cmdSetLevel:
            break;
        case CmdGetHelp:
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

void cmd_process::run()
{
    int len;

    while(1)
    {
        len = logger_loc_dev_tx_fifo_->read(rx_buffer_, DEVICE_RX_BUFFER_SIZE);
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
