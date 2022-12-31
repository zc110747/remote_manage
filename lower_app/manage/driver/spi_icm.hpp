//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi_icm.hpp
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
_Pragma("once")

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
}ICM_ADC_INFO;

typedef struct
{
    float gyro_x_act;     /*陀螺仪x轴角速度*/
    float gyro_y_act;     /*陀螺仪y轴角速度*/
    float gyro_z_act;     /*陀螺仪z轴角速度*/ 
    float accel_x_act;    /*加速度计x轴加速度*/
    float accel_y_act;    /*加速度计y轴加速度*/
    float accel_z_act;    /*加速度计z轴加速度*/
    float temp_act;       /*温度信息*/
}ICM_INFO;

class ICMDevice : public InfoBase<ICM_ADC_INFO>
{
private:
    ICM_INFO icm_info;

public:
    //constructor
    using InfoBase::InfoBase;
    void test();
    void ConvertInfo(void);

    ICM_INFO getConvertInfo(){
        return icm_info;
    }
};

