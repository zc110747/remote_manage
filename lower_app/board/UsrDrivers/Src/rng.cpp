
#include "rng.hpp"

bool rng_driver::init(void)
{
    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK)
    {
        return false;
    }
    return true;
}


uint32_t rng_driver::get_value(void)
{
    uint32_t value = 0;
    
    HAL_RNG_GenerateRandomNumber(&hrng, &value);
    
    return value;
}