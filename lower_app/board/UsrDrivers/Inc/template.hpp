_Pragma("once")

#include "includes.h"

class x_driver
{
public:
    BaseType_t init();

    static x_driver* get_instance(){
        static x_driver instance_;
        return &instance_;
    }  
public:


private:
    BaseType_t hardware_init(); 
    BaseType_t test();

private:
};