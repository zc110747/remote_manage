//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      cmd_process.cpp
//
//  Purpose:
//      用于支持命令行处理的接口, 包含字符串处理和事件触发
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
#include "logger_manage.hpp"

/*
!readdev    [index] #index=[0~3 led,beep,ap,icm]
!setdev     [index],[data] #index=[0~1 led,beep]  
!getNet     [index] #index=[0~2 udp,tcp,logger] 
!testDev    [index] #index=[0~3 led,beep,ap,icm]      
!getSerial  
!? or !help
*/
const static std::map<std::string, cmd_format_t> CmdMapM = {
    {"getos",      CmdGetOS},
    {"?",          cmdGetHelp},
    {"help",       cmdGetHelp},
};

const static std::map<cmd_format_t, std::string> CmdHelpMapM = {
    {CmdGetOS,   "!lower_dev getos"},
    {cmdGetHelp, "!lower_dev ?/help"},
};

bool cmd_process::parse_data(char *ptr, int size)
{
    if (ptr[0] != '!')
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "error command:%s", ptr);
        return false;
    }

    ptr[size] = '\0';

    //replace first ' ' by '\0'
    char *pStart = ptr;
    while ((*pStart != ' ') && (*pStart != '\0'))
        pStart++;
    pStart[0] = '\0';
    
    //将数据lower,解决数据不符合问题
    auto strVal = std::string(ptr);
    std::string strDst;
    strDst.resize(strVal.size());
    std::transform(strVal.begin(), strVal.end(), strDst.begin(), ::tolower);

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "rx command:%s", ptr);
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
        case CmdGetOS:
            {
                auto pSysConfig = system_config::get_instance();
                auto pVersion = pSysConfig->get_version().c_str();
                PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "FW_Version:%d, %d, %d, %d", pVersion[0], pVersion[1], pVersion[2], pVersion[3]);
                PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "Logger Level:%d ", (int)log_manage::get_instance()->get_level());
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
        default:
            ret = false;
            break;
    }

    if (!ret)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTimes(), "Invalid Formate:%d, data:%s", cmd_format_, cmd_data_pointer_);
    }
    return ret;
}