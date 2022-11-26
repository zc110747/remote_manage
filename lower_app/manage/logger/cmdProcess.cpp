//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      cmdProcess.cpp
//
//  Purpose:
//      cmdProcess done.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      8/8/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "cmdProcess.hpp"
#include "logger.hpp"
#include "driver.hpp"
#include "SystemConfig.hpp"
#include <algorithm>
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
    {"!readdev",    CmReadDev},
    {"!setdev",     CmSetDev},
    {"!testdev",    cmTestDev},
    {"!?",          CmGetHelp},
    {"!help",       CmGetHelp},
};

const static std::map<CmdFormat_t, std::string> CmdHelpMapM = {
    {CmdGetOS, "!getos"},
    {CmReadDev, "!readdev [index]"},
    {CmSetDev,  "!setdev [index],[data]"},
    {cmTestDev, "!testdev [index]"},
    {CmGetHelp, "!? ## !help"},
};

cmdProcess *cmdProcess::pInstance = nullptr;
cmdProcess* cmdProcess::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) cmdProcess();
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

bool cmdProcess::init()
{
    return true;
}

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

    if(CmdMapM.count(strDst) == 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "error command:%s", ptr);
        return false;
    }

    formatM = CmdMapM.find(strDst)->second;
    pDataM = pStart+1;
    
    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "right command:%d, data:%s", formatM, pDataM);
    return true;
}

bool cmdProcess::ProcessData()
{
    bool ret = true;
    switch(formatM)
    {
        case CmReadDev:
            { 
                DeviceReadInfo info = DeviceManageThread::getInstance()->getDeviceInfo();
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "LedStatus:%d!", info.led_io);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "beepStatus:%d!", info.beep_io);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "ApInfo, ir:%d, als:%d, ps:%d!",
                        info.ap_info.ir,
                        info.ap_info.als,
                        info.ap_info.ps);
                PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "ICMInfo, gx,gy,gz:%d,%d,%d;ax,ay,az:%d,%d,%d;temp:%d!",
                        info.icm_info.gyro_x_adc,
                        info.icm_info.gyro_y_adc,
                        info.icm_info.gyro_z_adc,
                        info.icm_info.accel_x_adc,
                        info.icm_info.accel_y_adc,
                        info.icm_info.accel_z_adc,
                        info.icm_info.temp_adc);
            }  
            break;
        case CmSetDev:
            break;
        case CmdGetOS:
            {
                auto pSysConfig = SystemConfig::getInstance();
                auto pVersion = pSysConfig->getversion();
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "FW_Version:%d, %d, %d, %d", pVersion[0], pVersion[1], pVersion[2], pVersion[3]);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "Sokcet Ipaddress:%s", pSysConfig->getudp()->ipaddr.c_str());
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "TCP Port:%d", pSysConfig->gettcp()->port);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "UDP Port:%d", pSysConfig->getudp()->port);
                PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "LOGGER Port:%d", pSysConfig->getlogger()->port);
            }
            break;
        case cmTestDev:
            { 
                char dev = pDataM[0];
                
                if(dev == '0')
                {
                    ledTheOne::getInstance()->test();
                }
            }
            break;
        case CmGetHelp:
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