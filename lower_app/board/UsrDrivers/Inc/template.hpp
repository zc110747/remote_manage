_Pragma("once")

#include "main.h"

class x_driver
{
public:
    bool init();

    static x_driver* get_instance(){
        static x_driver instance_;
        return &instance_;
    }  
public:


private:
    void hardware_init(); 
    bool test();

private:
};