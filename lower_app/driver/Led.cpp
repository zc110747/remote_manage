/*
 * File      : led.cpp
 * template file
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

#include "Led.h"

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
* Function
***************************************************************************/

/**
 * LED驱动需要初始化的信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
void LedDriveInit(void)
{
    pSystemConfigInfo = GetSSytemConfigInfo();

    LedStatusConvert((uint8_t)(pSystemConfigInfo->m_led0_status));
}

/**
 * LED开关转换函数
 * 
 * @param nBeepStatus 设置LED的开关状态
 *  
 * @return NULL
 */
void LedStatusConvert(uint8_t nLedStatus)
{
    int nFd;
    uint8_t nVal;
    ssize_t nSize;

    nFd = open(pSystemConfigInfo->m_dev_led.c_str(), O_RDWR | O_NDELAY);
    if(nFd != -1)
    {
        DRIVER_DEBUG("led write:%d\n", nLedStatus);
        nVal = nLedStatus;
        nSize = write(nFd, &nVal, 1);  //将数据写入LED
        if(nSize != 0)
        {
            DRIVER_DEBUG("Write failed\n");
        }
        close(nFd);
    }
    else
    {
        DRIVER_DEBUG("led open %s failed, val:%d!\n", pSystemConfigInfo->m_dev_led.c_str(), nLedStatus);
    }
    
}

/**
 * 获取LED状态
 * 
 * @param nBeepStatus 获取LED状态
 *  
 * @return LED的状态
 */
uint8_t LedStatusRead(void)
{
    int nFd;
    uint8_t nValue = 0;
    ssize_t nSize;

    nFd = open(pSystemConfigInfo->m_dev_led.c_str(), O_RDONLY | O_NDELAY);
    if(nFd != -1)
    {
        nSize = read(nFd, &nValue, 1);  //将数据写入LED
        if(nSize != 0)
        {
            DRIVER_DEBUG("Read failed\n");
        }
        close(nFd);
    }
    return nValue;
}
