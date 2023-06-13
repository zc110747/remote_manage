//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      adc.hpp
//
//  Purpose:
//     adc driver normal get.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include "main.h"
#include "driver.hpp"

#define ADC_AVG_TIMES   5

class adc_driver
{
public:
    BaseType_t init(void);

    uint16_t get_adc_value(uint32_t channel);
    uint16_t get_adc_avg(uint32_t channel);

    static adc_driver* get_instance(){
        static adc_driver instance_;
        return &instance_;
    }

private:
    BaseType_t hardware_init(void);

    ADC_HandleTypeDef adc1_hander_;
};