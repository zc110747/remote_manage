_Pragma("once")

#include "main.h"
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
    void hardware_init(); 
    BaseType_t test();

private:
};