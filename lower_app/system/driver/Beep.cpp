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
static int beep_fd;

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
 * 配置蜂鸣器的驱动
 * 
 * @param NULL
 *  
 * @return NULL
 */
void BeepDriveInit(void)
{
    struct SSystemConfig *pSystemConfigInfo;
    pSystemConfigInfo = GetSSytemConfigInfo();

    beep_fd = open(pSystemConfigInfo->m_dev_beep.c_str(), O_RDWR | O_NDELAY);
    if(beep_fd != -1)
    {
        BeepStatusConvert((uint8_t)(pSystemConfigInfo->m_beep0_status));
    }
    else
    {
        DRIVER_DEBUG("Beep Open %s Failed, Error:%s\n", 
                    pSystemConfigInfo->m_dev_beep.c_str(), strerror(errno));
    }   
}

/**
 * 释放蜂鸣器应用资源
 * 
 * @param NULL
 *  
 * @return NULL
 */
void BeepDriverRelease(void)
{
    close(beep_fd);
}

/**
 * 修改蜂鸣器的当前状态
 * 
 * @param nBeepStatus 配置蜂鸣器的开关状态
 *  
 * @return NULL
 */
void BeepStatusConvert(uint8_t nBeepStatus)
{
    uint8_t nVal;
    ssize_t nSize;

    if(beep_fd != -1)
    {
        DRIVER_DEBUG("Beep Write:%d\n", nBeepStatus);
        nVal = nBeepStatus;
        nSize = write(beep_fd, &nVal, 1);  //将数据写入LED
        if(nSize < 0)
        {
            DRIVER_DEBUG("Write Failed\n");
        }
    }
    else
    {
        DRIVER_DEBUG("Beep Open Failed, Write Val:%d!\n", nBeepStatus);
    }
}

/**
 * 获取蜂鸣器当前的状态
 * 
 * @param nBeepStatus NULL
 *  
 * @return 蜂鸣器的当前工作状态
 */
uint8_t BeepStatusRead(void)
{
    uint8_t nValue = 0;
    ssize_t nSize;

    if(beep_fd != -1)
    {
        nSize = read(beep_fd, &nValue, 1);  //读取Beep的值
        if(nSize < 0)
        {
            DRIVER_DEBUG("Beep Read Failed, Error:%s\n", strerror(errno));
        }
    }
    else
    {
        //DRIVER_DEBUG("Beep Open Failed, Read Error!\n");
    }
    
    return nValue;
}

