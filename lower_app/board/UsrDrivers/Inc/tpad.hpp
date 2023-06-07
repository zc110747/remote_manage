_Pragma("once")

#include "main.h"

class tpad_driver
{
public:
    void init();

    static tpad_driver* get_instance(){
        static tpad_driver instance_;
        return &instance_;
    }  
    uint8_t scan_key();

public:
    uint16_t get_current_value()    {return current_value_;}
    uint16_t get_no_push_value()    {return no_push_value_;}

private:
    void hardware_init(); 
    uint16_t get_value();
    uint16_t get_max_value();
    void reset();
    bool test();

private:
    TIM_HandleTypeDef timer_handler_;
    uint16_t no_push_value_{0};
    uint16_t current_value_;
};