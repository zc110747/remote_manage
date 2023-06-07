
#include "rng.hpp"

bool rng_driver::init(void)
{
    rng_handler_.Instance = RNG;
    if (HAL_RNG_Init(&rng_handler_) != HAL_OK)
    {
        return false;
    }
    return true;
}


uint32_t rng_driver::get_value(void)
{
    uint32_t value = 0;
    
    HAL_RNG_GenerateRandomNumber(&rng_handler_, &value);
    
    return value;
}