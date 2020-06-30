/*
 * File      : template.cpp
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
static struct SSpiInfo spi_info;
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
 * 读取icm20608(spi接口)的状态信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
SSpiInfo *SpiDevInfoRead(void)
{
    int nFd;
    uint8_t nValue = 0;
    ssize_t nSize;
    uint32_t databuf[7];

    pSystemConfigInfo = GetSSytemConfigInfo();
    nFd = open(pSystemConfigInfo->m_dev_icm_spi.c_str(), O_RDWR);
    if(nFd != -1)
    {
        nSize = read(nFd, databuf, sizeof(databuf));
        if(nSize > 0)
        {
           	spi_info.gyro_x_adc = databuf[0];
			spi_info.gyro_y_adc = databuf[1];
			spi_info.gyro_z_adc = databuf[2];
			spi_info.accel_x_adc = databuf[3];
			spi_info.accel_y_adc = databuf[4];
			spi_info.accel_z_adc = databuf[5];
			spi_info.temp_adc = databuf[6];
            printf("\r\n原始值:\r\n");
			printf("gx = %d, gy = %d, gz = %d\r\n", spi_info.gyro_x_adc, spi_info.gyro_y_adc, spi_info.gyro_z_adc);
			printf("ax = %d, ay = %d, az = %d\r\n", spi_info.accel_x_adc, spi_info.accel_y_adc, spi_info.accel_z_adc);
			printf("temp = %d\r\n", spi_info.temp_adc);
        }
        else
        {
            USR_DEBUG("read spi device failed, error:%s\n", strerror(errno));
        }    
        close(nFd);
    }
    else
    {
        USR_DEBUG("open %s failed, error:%s\n", pSystemConfigInfo->m_dev_icm_spi.c_str(), strerror(errno));
    }
    
    return &spi_info;
}
