//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     i2c.cpp
//
//  Purpose:
//     i2c driver.
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

#define PCF8574_ADDR 	        0x40
#define PCF8574_I2C_TIMEOUT     100

class i2c_driver
{
public:
    BaseType_t init();

    static i2c_driver* get_instance(){
        static i2c_driver instance_;
        return &instance_;
    }  
public:
    void write_io(uint8_t pin, uint8_t status); 
    BaseType_t write_i2c(uint8_t addr, uint8_t data);
    BaseType_t read_i2c(uint8_t addr, uint8_t *pdata);
    
private:
    BaseType_t hardware_init(); 
    bool test();
   
private:
    I2C_HandleTypeDef i2c2_handler_;
};