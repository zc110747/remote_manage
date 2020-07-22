/*
 * File      : IcmSpi.cpp
 * Icm20608-spi应用层驱动实现
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
#include "ApI2c.h"
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
static int i2c_fd;

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
 * 配置ap3216-i2c的驱动
 * 
 * @param NULL
 *  
 * @return NULL
 */
void I2cDriverInit(void)
{
    struct SSystemConfig *pSystemConfigInfo;

    pSystemConfigInfo = GetSSytemConfigInfo();
    i2c_fd = open(pSystemConfigInfo->m_dev_ap_i2c.c_str(), O_RDWR);
    if(i2c_fd == -1)
    {
        DRIVER_DEBUG("Deivce %s Open Failed, Error:%s\n", 
                    pSystemConfigInfo->m_dev_ap_i2c.c_str(), strerror(errno));
    }
}

/**
 * 释放ap3216-i2c应用资源
 * 
 * @param NULL
 *  
 * @return NULL
 */
void I2cDriverRelease(void)
{
    close(i2c_fd);
}

/**
 * 读取ap3216(spi接口)的状态信息
 * 
 * @param pI2cInfo i2c读取ap3216态信息
 *  
 * @return i2c读取设备的状态
 */
int I2cDevInfoRead(SApInfo *pI2cInfo)
{
    uint8_t nValue = 0;
    ssize_t nSize;
    uint16_t databuf[3];

    if(i2c_fd != -1)
    {
        nSize = read(i2c_fd, databuf, sizeof(databuf));
        if(nSize >= 0)
        {
           	pI2cInfo->ir = databuf[0];
			pI2cInfo->als = databuf[1];
			pI2cInfo->ps = databuf[2];
            printf("\r\n原始值:\r\n");
			printf("ir = %d, als = %d, ps = %d\r\n", pI2cInfo->ir, 
                    pI2cInfo->als, pI2cInfo->ps);
        }
        else
        {
            USR_DEBUG("read i2c device failed, error:%s\n", strerror(errno));
            return RT_INVALID;
        }    
    }
    else
    {
        return RT_INVALID;
    }
    
    return RT_OK;
}
