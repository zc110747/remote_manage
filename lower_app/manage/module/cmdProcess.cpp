//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      cmdProcess.cpp
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

#include "cmdProcess.hpp"
#include "logger.hpp"
#include "DeviceManageThread.hpp"

/*
!readdev    [index] #index=[0~3 led,beep,ap,icm]
!setdev     [index],[data] #index=[0~1 led,beep]  
!getNet     [index] #index=[0~2 udp,tcp,logger] 
!testDev    [index] #index=[0~3 led,beep,ap,icm]      
!getSerial  
!? or !help
*/
const static std::map<std::string, CmdFormat_t> CmdMapM = {
    {"!getos",      CmdGetOS},
    {"!readdev",    CmdReadDev},
    {"!setdev",     CmdSetDev},
    {"!setlevel",   cmdSetLevel},
    {"!connect",    CmdConnect},
    {"!?",          CmdGetHelp},
    {"!help",       CmdGetHelp},
};

const static std::map<CmdFormat_t, std::string> CmdHelpMapM = {
    {CmdGetOS,   "!getos"},
    {CmdReadDev, "!readdev"},
    {CmdSetDev,  "!setdev [index],[action]"},
    {cmdSetLevel, "!setlevel [lev 0-5]",},
    {CmdGetHelp, "!? ## !help"},
};

bool cmdProcess::parseData(char *ptr, int size)
{
    if(ptr[0] != '!')
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "error command:%s", ptr);
        return false;
    }

    ptr[size] = '\0';

    //replace first ' ' by '\0'
    char *pStart = ptr;
    while((*pStart != ' ') && (*pStart != '\0'))
        pStart++;
    pStart[0] = '\0';
    
    //将数据lower,解决数据不符合问题
    auto strVal = std::string(ptr);
    std::string strDst;
    strDst.resize(strVal.size());
    std::transform(strVal.begin(), strVal.end(), strDst.begin(), ::tolower);

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "rx command:%s", ptr);
    if(CmdMapM.count(strDst) == 0)
    {
        return false;
    }

    formatM = CmdMapM.find(strDst)->second;
    pDataM = pStart+1;
    return true;
}

bool cmdProcess::ProcessData()
{
    bool ret = true;
    switch(formatM)
    {
        case CmdReadDev:
            { 
                auto info = NAMESPACE_DEVICE::DeviceManageThread::getInstance()->getDeviceInfo();
                
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "LedStatus:%d!", info.led_io);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "beepStatus:%d!", info.beep_io);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "ApInfo, ir:%d, als:%d, ps:%d!",
                        info.ap_info.ir,
                        info.ap_info.als,
                        info.ap_info.ps);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "ICMInfo, gx,gy,gz:%.2f,%.2f,%.2f;ax,ay,az:%.2f,%.2f,%.2f;temp:%.2f!",
                        info.icm_info.gyro_x_act,
                        info.icm_info.gyro_y_act,
                        info.icm_info.gyro_z_act,
                        info.icm_info.accel_x_act,
                        info.icm_info.accel_y_act,
                        info.icm_info.accel_z_act,
                        info.icm_info.temp_act);
            }  
            break;
        case CmdGetOS:
            {
                auto pSysConfig = SystemConfig::getInstance();
                auto pVersion = pSysConfig->getversion();
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "FW_Version:%d, %d, %d, %d", pVersion[0], pVersion[1], pVersion[2], pVersion[3]);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Sokcet Ipaddress:%s", pSysConfig->getudp()->ipaddr.c_str());
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Port TCP:%d, UDP: %d, LOGGER:%d ", 
                    pSysConfig->gettcp()->port, pSysConfig->getudp()->port, pSysConfig->getlogger()->port);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Logger Level:%d ", (int)LoggerManage::getInstance()->getlevel());
            }
            break;
        case CmdSetDev:
            {
                uint32_t device = 0, action = 0;
                sscanf(pDataM, "%d,%d", &device, &action);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "SetDev:%d, %d!", device, action);
                NAMESPACE_DEVICE::DeviceManageThread::getInstance()->sendHardProcessMsg(device, action);
            }
            break;
        case cmdSetLevel:
            { 
                uint8_t level = pDataM[0] - '0';
                if(level > 5)
                    level = 5;
                LoggerManage::getInstance()->setlevel((LOG_LEVEL)level);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Set Logger Level:%d!", level);
            }
            break;
        case CmdConnect:
            {
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Connect with Remote Success!");
            }
            break;
        case CmdGetHelp:
            {
                for(auto &[x, y] : CmdHelpMapM)
                {
                    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), y.c_str());
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                }
            }
            break;
        default:
            ret = false;
            break;
    }

    if(!ret)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Invalid Formate:%d, data:%s", formatM, pDataM);
    }
    return ret;
}