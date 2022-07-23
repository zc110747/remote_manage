/*
 * File      : IcmSpi.h
 * Icm20608-spi应用层驱动实现接口
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
#ifndef _INCLUDE_ICM_SPI_H
#define _INCLUDE_ICM_SPI_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "../include/productConfig.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/
struct SSpiInfo
{
    /*陀螺仪x轴角速度*/
    int gyro_x_adc;

    /*陀螺仪y轴角速度*/
    int gyro_y_adc;

    /*陀螺仪z轴角速度*/
    int gyro_z_adc;

    /*加速度计x轴加速度*/
    int accel_x_adc;

    /*加速度计y轴加速度*/
    int accel_y_adc;

    /*加速度计z轴加速度*/
    int accel_z_adc;

    /*温度信息*/
    int temp_adc;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/

/*配置icm20608-spi的驱动*/
void SpiDriverInit(void);

/*释放icm20608-spi应用资源*/
void SpiDriverRelease(void);

/*读取icm20608(spi接口)的状态信息*/
int SpiDevInfoRead(SSpiInfo *pSpiInfo);
#endif
