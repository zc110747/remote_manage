//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi_icm.cpp
//
//  Purpose:
//      ICM模块对应的SPI接口，通过继承支持直接读取模块信息
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
#include "spi_icm.hpp"

void ICMDevice::test()
{
    
}

void ICMDevice::ConvertInfo(void)
{
    icm_info.gyro_x_act = (float)(data.info.gyro_x_adc)/16.4;
    icm_info.gyro_y_act = (float)(data.info.gyro_y_adc)/16.4;
    icm_info.gyro_z_act = (float)(data.info.gyro_z_adc)/16.4;
    icm_info.accel_x_act = (float)(data.info.accel_x_adc)/2048;
    icm_info.accel_y_act = (float)(data.info.accel_y_adc)/2048;
    icm_info.accel_z_act = (float)(data.info.accel_z_adc)/2048;
    icm_info.temp_act = ((float)(data.info.temp_adc) - 25 ) / 326.8 + 25;
}