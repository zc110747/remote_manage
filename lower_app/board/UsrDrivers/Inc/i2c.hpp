_Pragma("once")

#include "main.h"
#include "includes.hpp"

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
    BaseType_t write_i2c(uint8_t data);
    BaseType_t read_i2c(uint8_t *pdata);
    
private:
    void hardware_init(); 
    bool test();
    BaseType_t pcf8575_init();
   
private:
    I2C_HandleTypeDef i2c2_handler_;
};