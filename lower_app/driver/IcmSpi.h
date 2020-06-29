/*
 * File      : IcmSpi.h
 * This file is for ICM interface
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
#include "../include/UsrTypeDef.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/
struct SSpiInfo
{
    int gyro_x_adc;
    int gyro_y_adc;
    int gyro_z_adc;
    int accel_x_adc;
    int accel_y_adc;
    int accel_z_adc;
    int temp_adc;
};

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
void SpiDriverInit(void);
SSpiInfo *SpiDevInfoRead(void);
#endif
