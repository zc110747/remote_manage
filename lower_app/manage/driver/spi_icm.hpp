//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi_icm.hpp
//
//  Purpose:
//      spi read icm interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _SPI_ICM_HPP
#define _SPI_ICM_HPP

#include "deviceBase.hpp"

typedef struct
{
    int gyro_x_adc;     /*陀螺仪x轴角速度*/
    int gyro_y_adc;     /*陀螺仪y轴角速度*/
    int gyro_z_adc;     /*陀螺仪z轴角速度*/ 
    int accel_x_adc;    /*加速度计x轴加速度*/
    int accel_y_adc;    /*加速度计y轴加速度*/
    int accel_z_adc;    /*加速度计z轴加速度*/
    int temp_adc;       /*温度信息*/
}ICM_INFO;

class ICMDevice : public InfoBase<ICM_INFO>
{
private:
    static ICMDevice *pInstance;

public:
    //constructor
    using InfoBase::InfoBase;

    static ICMDevice *getInstance();
    void release();
};
#endif
