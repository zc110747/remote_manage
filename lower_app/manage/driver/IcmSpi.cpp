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
#include "IcmSpi.h"
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
static int spi_fd;

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
 * 配置icm20608-spi的驱动
 * 
 * @param NULL
 *  
 * @return NULL
 */
void SpiDriverInit(void)
{
    struct SSystemConfig *pSystemConfigInfo;

    pSystemConfigInfo = GetSSytemConfigInfo();
    spi_fd = open(pSystemConfigInfo->m_dev_icm_spi.c_str(), O_RDWR);
    if(spi_fd == -1)
    {
        DRIVER_DEBUG("Spi Open %s Failed, Error:%s\n", 
                    pSystemConfigInfo->m_dev_icm_spi.c_str(), strerror(errno));
    }
}

/**
 * 释放icm20608-spi应用资源
 * 
 * @param NULL
 *  
 * @return NULL
 */
void SpiDriverRelease(void)
{
    close(spi_fd);
}

/**
 * 读取icm20608(spi接口)的状态信息
 * 
 * @param pSpiInfo SPI读取icm20608状态信息
 *  
 * @return Spi读取设备的状态
 */
int SpiDevInfoRead(SSpiInfo *pSpiInfo)
{
    ssize_t nSize;
    uint32_t databuf[7];

    if(spi_fd != -1)
    {
        nSize = read(spi_fd, databuf, sizeof(databuf));
        if(nSize >= 0)
        {
           	pSpiInfo->gyro_x_adc = databuf[0];
			pSpiInfo->gyro_y_adc = databuf[1];
			pSpiInfo->gyro_z_adc = databuf[2];
			pSpiInfo->accel_x_adc = databuf[3];
			pSpiInfo->accel_y_adc = databuf[4];
			pSpiInfo->accel_z_adc = databuf[5];
			pSpiInfo->temp_adc = databuf[6];
            // printf("\r\n原始值:\r\n");
			// printf("gx = %d, gy = %d, gz = %d\r\n", pSpiInfo->gyro_x_adc, 
            //         pSpiInfo->gyro_y_adc, pSpiInfo->gyro_z_adc);
			// printf("ax = %d, ay = %d, az = %d\r\n", pSpiInfo->accel_x_adc, 
            //         pSpiInfo->accel_y_adc, pSpiInfo->accel_z_adc);
			// printf("temp = %d\r\n", pSpiInfo->temp_adc);
        }
        else
        {
            USR_DEBUG("read spi device failed, error:%s, nSize:%d\n", strerror(errno), (int)nSize);
            return RT_INVALID;
        }    
    }
    else
    {
        //USR_DEBUG("open %s failed, error:%s\n", pSystemConfigInfo->m_dev_icm_spi.c_str(), strerror(errno));
        return RT_INVALID;
    }
    
    return RT_OK;
}
