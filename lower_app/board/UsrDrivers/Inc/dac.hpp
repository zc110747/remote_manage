//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      dac.hpp
//
//  Purpose:
//      dac interface init and set_voltage.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"

//reference voltage, uint:mv
#define DAC_REFERENCE_VOL   3300

//dac max output value
#define DAC_MAX_VALUE       4096

class dac_driver
{
public:
    BaseType_t init();

    static dac_driver* get_instance(){
        static dac_driver instance_;
        return &instance_;
    }  
public:
    void set_voltage(uint16_t mv);

private:
    BaseType_t hardware_init(); 
    BaseType_t test();

private:
    DAC_HandleTypeDef dac_handle_;
};