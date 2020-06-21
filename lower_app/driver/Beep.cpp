/*
 * File      : beep.cpp
 * beep driver application
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/

#include "Beep.h"
#include "../include/SystemConfig.h"

/**************************************************************************
* Local Macro Definition
***************************************************************************/

/**************************************************************************
* Local Type Definition
***************************************************************************/

/**************************************************************************
* Local static Variable Declaration
***************************************************************************/
static struct SSystemConfig *pSystemConfigInfo;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Local Function Declaration
***************************************************************************/

/**************************************************************************
* Local Function
***************************************************************************/

/**************************************************************************
* Function
***************************************************************************/
/**
 * BEEP驱动需要初始化的信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
void BeepDriveInit(void)
{
    pSystemConfigInfo = GetSSytemConfigInfo();

    BeepStatusConvert((uint8_t)(pSystemConfigInfo->m_beep0_status));
}

/**
 * beep开关转换函数
 * 
 * @param nBeepStatus 设置蜂鸣器的开关状态
 *  
 * @return NULL
 */
void BeepStatusConvert(uint8_t nBeepStatus)
{
    int nFd;
    uint8_t nVal;
    ssize_t nSize;

    nFd = open(pSystemConfigInfo->m_dev_beep.c_str(), O_RDWR | O_NDELAY);
    if(nFd != -1)
    {
        DRIVER_DEBUG("beep write:%d\n", nBeepStatus);
        nVal = nBeepStatus;
        nSize = write(nFd, &nVal, 1);  //将数据写入LED
        if(nSize < 0)
        {
            DRIVER_DEBUG("Write failed\n");
        }
        close(nFd);
    }
    else
    {
        DRIVER_DEBUG("beep open %s failed, val:%d!\n", pSystemConfigInfo->m_dev_led.c_str(), nBeepStatus);
    }
}

/**
 * 获取BEEP开关状态
 * 
 * @param nBeepStatus 获取BEEP状态
 *  
 * @return BEEP的开关状态
 */
uint8_t BeepStatusRead(void)
{
    int nFd;
    uint8_t nValue = 0;
    ssize_t nSize;

    nFd = open(pSystemConfigInfo->m_dev_beep.c_str(), O_RDONLY | O_NDELAY);
    if(nFd != -1)
    {
        nSize = read(nFd, &nValue, 1);  //读取Beep的值
        if(nSize < 0)
        {
            DRIVER_DEBUG("Beep Read failed, error:%s\n", strerror(errno));
        }
        close(nFd);
    }
    return nValue;
}