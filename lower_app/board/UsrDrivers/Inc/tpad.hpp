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
    uint8_t scan_key(uint8_t mode=0);
    
private:
    void hardware_init(uint32_t arr, uint16_t psc); 
    uint16_t get_value();
    uint16_t get_max_value(uint8_t n);
    void reset();
    bool test();

private:
    TIM_HandleTypeDef TIM2_Handler;
    volatile uint16_t default_val{0};
};