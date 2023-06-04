_Pragma("once")

#include "main.h"
#include "includes.hpp"

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
    void hardware_init(); 
    BaseType_t test();

private:
    DAC_HandleTypeDef dac_handle_;
};