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

/*
command need sequrate with space
!readDev [index]        #index=[0, led], [1, beep], [2, i2c_ap], [3, spi_icm]
!setDev [index],[data]  #index=[0, led], [1, beep]   
!getNet x               #x can be any
!getSerial x            #...
*/
const char *cmdProcessPtr[] = {
    "!readDev",
    "!setDev",
    "!getNet",
    "!getSerial",
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
    static_assert((sizeof(cmdProcessPtr)/sizeof(char *)) == COMMAND_MAX_SIZE, "Invalid cmdProcessPtr list");

    for(int index=0; index<COMMAND_MAX_SIZE; index++)
    {
        mapM.insert(std::pair<std::string, CmdFormat_t>(std::string(cmdProcessPtr[index]), (CmdFormat_t)index));
    }
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
    while((*pStart) != ' ')
        pStart++;
    pStart[0] = '\0';
    
    auto strVal = std::string(ptr);
    if(mapM.count(strVal) == 0)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "error command:%s", ptr);
        return false;
    }

    formatM = mapM[strVal];
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
                    PRINT_LOG(LOG_ERROR, xGetCurrentTime(), "Command Invalid Device:%c", dev);
                    ret = false;
                }
            }  
            break;
        case CmSetDev:
            break;
        case CmGetNet:
            break;
        case CmGetSer:
            break;
        default:
            ret = false;
            break;
    }
    return ret;
}