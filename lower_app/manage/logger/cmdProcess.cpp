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
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      8/8/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "cmdProcess.hpp"
#include "logger.hpp"
#include "../driver/driver.hpp"
#include "../source/SystemConfig.hpp"
#include <algorithm>

/*
!readdev    [index] #index=[0~3 led,beep,ap,icm]
!setdev     [index],[data] #index=[0~1 led,beep]  
!getNet     [index] #index=[0~2 udp,tcp,logger] 
!testDev    [index] #index=[0~3 led,beep,ap,icm]      
!getSerial  
!? or !help
*/
const static std::map<std::string, CmdFormat_t> CmdMapM = {
    {"!readdev",    CmReadDev},
    {"!setdev",     CmSetDev},
    {"!getnet",     CmGetNet},
    {"!getserial",  CmGetSer},
    {"!testdev",    cmTestDev},
    {"!?",          CmGetHelp},
    {"!help",       CmGetHelp},
};

const static std::map<CmdFormat_t, std::string> CmdHelpMapM = {
    {CmReadDev, "!readdev [index]"},
    {CmSetDev,  "!setdev [index],[data]"},
    {CmGetNet,  "!getNet [index]"},
    {CmGetSer,  "!getSerial"},
    {cmTestDev, "!testdev [index]"},
    {CmGetHelp, "!? ## !help"},
};

cmdProcess::cmdProcess()
{
}

cmdProcess::~cmdProcess()
{
}

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
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "error command:%s", ptr);
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
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "error command:%s", ptr);
        return false;
    }

    formatM = CmdMapM.find(strDst)->second;
    pDataM = pStart+1;
    
    PRINT_LOG(LOG_INFO, xGetCurrentTime(), "right command:%d, data:%s", formatM, pDataM);
    return true;
}

bool cmdProcess::ProcessData()
{
    bool ret = true;
    switch(formatM)
    {
        case CmReadDev:
            { 
                char dev = pDataM[0];
                if(dev == '0')
                {
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), "LedStatus:%d!",ledTheOne::getInstance()->getIoStatus());
                }
                else if(dev == '1')
                {
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), "beepStatus:%d!",beepTheOne::getInstance()->getIoStatus());
                }
                else if(dev == '2')
                {
                    AP_INFO *pInfo = APDevice::getInstance()->getInfo();
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), "ApInfo, ir:%d, als:%d, ps:%d!",
                        pInfo->ir,
                        pInfo->als,
                        pInfo->ps);
                }
                else if(dev == '3')
                {
                    ICM_INFO *pInfo = ICMDevice::getInstance()->getInfo();
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), "ICMInfo, gx,gy,gz:%d,%d,%d;ax,ay,az:%d,%d,%d;temp:%d!",
                        pInfo->gyro_x_adc,
                        pInfo->gyro_y_adc,
                        pInfo->gyro_z_adc,
                        pInfo->accel_x_adc,
                        pInfo->accel_y_adc,
                        pInfo->accel_z_adc,
                        pInfo->temp_adc);
                }
                else
                {
                    ret = false;
                }
            }  
            break;
        case CmSetDev:
            break;
        case CmGetNet:
            {
                char net = pDataM[0];
                if(net == '0')
                {
                    auto pSocket = SystemConfig::getInstance()->getudp();
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), 
                        "UDP Sokcet, Ipaddress:%s, port:%d", pSocket->ipaddr.c_str(), pSocket->port);
                }
                else if(net == '1')
                {
                    auto pSocket = SystemConfig::getInstance()->gettcp();
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), 
                        "TCP Sokcet, Ipaddress:%s, port:%d", pSocket->ipaddr.c_str(), pSocket->port);
                }
                else if(net == '2')
                {
                    auto pSocket = SystemConfig::getInstance()->getlogger();
                    PRINT_LOG(LOG_FATAL, xGetCurrentTime(), 
                        "Logger Sokcet, Ipaddress:%s, port:%d", pSocket->ipaddr.c_str(), pSocket->port);
                }
                else
                {
                    ret = false;
                }
            }
            break;
        case CmGetSer:
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
                // std::for_each(CmdHelpMapM.begin(), CmdHelpMapM.end(), [](auto &ref_value){
                //     PRINT_LOG(LOG_INFO, xGetCurrentTime(), ref_value.second.c_str());
                // });
                
                for(auto &ref : CmdHelpMapM)
                {
                    PRINT_LOG(LOG_INFO, xGetCurrentTime(), ref.second.c_str());
                    usleep(1000);
                }
            }
            break;
        default:
            ret = false;
            break;
    }

    if(!ret)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Invalid Formate:%d, data:%s", formatM, pDataM);
    }
    return ret;
}