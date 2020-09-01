/*
 * File      : led.cpp
 * Led应用层驱动实现
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
static int led_fd;

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
 * 配置LED的驱动
 * 
 * @param NULL
 *  
 * @return NULL
 */
void LedDriveInit(void)
{
    struct SSystemConfig *pSystemConfigInfo;
    pSystemConfigInfo = GetSSytemConfigInfo();
 
    led_fd = open(pSystemConfigInfo->m_dev_led.c_str(), O_RDWR | O_NDELAY);
    if(led_fd != -1)
    {
        LedStatusConvert((uint8_t)(pSystemConfigInfo->m_led0_status));
    }
    else
    {
        DRIVER_DEBUG("Led Open %s Failed, Error:%s!\n",
                    pSystemConfigInfo->m_dev_led.c_str(), strerror(errno));
    }
}

/**
 * 释放LED应用资源
 * 
 * @param NULL
 *  
 * @return NULL
 */
void LedDriverRelease(void)
{
    close(led_fd);
}

/**
 * 修改LED的当前状态
 * 
 * @param nBeepStatus 设置LED的开关状态
 *  
 * @return NULL
 */
void LedStatusConvert(uint8_t nLedStatus)
{
    uint8_t nVal;
    ssize_t nSize;

    if(led_fd != -1)
    {
        DRIVER_DEBUG("Led Write:%d\n", nLedStatus);
        nVal = nLedStatus;
        nSize = write(led_fd, &nVal, 1);  //将数据写入LED
        if(nSize < 0)
        {
            DRIVER_DEBUG("Write Failed\n");
        }
    }
    else
    {
        DRIVER_DEBUG("Led Open Failed, Write Val:%d!\n", nLedStatus);
    }
}

/**
 * 获取LED当前的状态
 * 
 * @param nBeepStatus 获取LED状态
 *  
 * @return LED的状态
 */
uint8_t LedStatusRead(void)
{
    uint8_t nValue = 0;
    ssize_t nSize;

    if(led_fd != -1)
    {
        nSize = read(led_fd, &nValue, 1);  //将数据写入LED
        if(nSize < 0)
        {
            DRIVER_DEBUG("Led Read Failed, Error:%s\n", strerror(errno));
        }
    }
    else
    {
        //DRIVER_DEBUG("Led Open Failed, Read Error!\n");
    }
    
    return nValue;
}
