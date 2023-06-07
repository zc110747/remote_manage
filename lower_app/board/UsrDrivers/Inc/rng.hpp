
#pragma once

#include "main.h"
#include "driver.hpp"

class rng_driver
{
public:
    bool init(void);

    static rng_driver *get_instance(void)
    {
        static rng_driver instance;
        return &instance;
    }
    
    uint32_t get_value(void);
    
private:
    RNG_HandleTypeDef rng_handler_;
};