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
 * 打开SPI设备
 * 
 * @param NULL
 *  
 * @return NULL
 */
void SpiDriverInit(void)
{
    static struct SSystemConfig *pSystemConfigInfo;
    pSystemConfigInfo = GetSSytemConfigInfo();
    spi_fd = open(pSystemConfigInfo->m_dev_icm_spi.c_str(), O_RDWR);

    if(spi_fd < 0){
        USR_DEBUG("open spi device %s failed, error:%s\n", pSystemConfigInfo->m_dev_icm_spi.c_str(), strerror(errno));
    }
    else
    {
        USR_DEBUG("open spi device %s success, id:%d\n", pSystemConfigInfo->m_dev_icm_spi.c_str(), spi_fd);
    }
    
}

/**
 * 读取icm20608(spi接口)的状态信息
 * 
 * @param NULL
 *  
 * @return NULL
 */
SSpiInfo *SpiDevInfoRead(void)
{
    uint8_t nValue = 0;
    ssize_t nSize = 0;
    int databuf[7];

    if(spi_fd != -1)
    {
        nSize = read(spi_fd, databuf, sizeof(databuf));
        if(nSize == 0)
        {
           	spi_info.gyro_x_adc = databuf[0];
			spi_info.gyro_y_adc = databuf[1];
			spi_info.gyro_z_adc = databuf[2];
			spi_info.accel_x_adc = databuf[3];
			spi_info.accel_y_adc = databuf[4];
			spi_info.accel_z_adc = databuf[5];
			spi_info.temp_adc = databuf[6];
        }
        else if(nSize < 0)
        {
            USR_DEBUG("read spi %d device failed, error:%s\n", spi_fd, strerror(errno));
        } 
    }
    else
    {
        USR_DEBUG("open spi device failed, error:%s\n", strerror(errno));
    }
    
    return &spi_info;
}
