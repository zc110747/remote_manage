_Pragma("once")

#include "main.h"

class i2c_driver
{
public:
    bool init();

    static i2c_driver* get_instance(){
        static i2c_driver instance_;
        return &instance_;
    }  
public:


private:
    void hardware_init(); 
    bool test();

private:
};