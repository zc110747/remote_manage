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
#include <math.h>

#define ANGLE_LIST_SIZE 3

uint8_t AngleIndex = 0;
double AngleList[ANGLE_LIST_SIZE];

void icm_device::calculate_angle(void)
{
    double min, max, sumx;

    icm_info_.gyro_x_act = (float)(data.info.gyro_x_adc)/16.4;
    icm_info_.gyro_y_act = (float)(data.info.gyro_y_adc)/16.4;
    icm_info_.gyro_z_act = (float)(data.info.gyro_z_adc)/16.4;
    icm_info_.accel_x_act = (float)(data.info.accel_x_adc);
    icm_info_.accel_y_act = (float)(data.info.accel_y_adc);
    icm_info_.accel_z_act = (float)(data.info.accel_z_adc);
    icm_info_.temp_act = ((float)(data.info.temp_adc) - 25 ) / 326.8 + 25;

    AngleList[AngleIndex] = -atan2((double)icm_info_.accel_x_act, (double)icm_info_.accel_z_act)*18000/3.141592653;
    if(AngleList[AngleIndex] > 18000)
        AngleList[AngleIndex] -= 36000;
    if(AngleList[AngleIndex] < -18000)
        AngleList[AngleIndex] += 36000;

    AngleIndex++;
    if(AngleIndex == ANGLE_LIST_SIZE)
        AngleIndex = 0;
    
    sumx = 0;
    min = 0x3fff;
    max = -0x3fff;

    for(int i=0; i<ANGLE_LIST_SIZE; i++)
    {
        sumx += AngleList[i];
        if(AngleList[i] >= max)
            max = AngleList[i];
        
        if(AngleList[i] <= min)
            min = AngleList[i];
    }
    sumx = sumx - max - min;
    angle_ = ((int32_t)round(sumx/(ANGLE_LIST_SIZE-2)))/100;
}