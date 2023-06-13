//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     rng.cpp
//
//  Purpose:
//     rng driver.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
#include "rng.hpp"

BaseType_t rng_driver::init(void)
{
    rng_handler_.Instance = RNG;
    if (HAL_RNG_Init(&rng_handler_) != HAL_OK)
    {
        printf("rng init failed\r\n");
        return pdFAIL;
    }
    return pdPASS;
}

uint32_t rng_driver::get_value(void)
{
    uint32_t value = 0;
    
    HAL_RNG_GenerateRandomNumber(&rng_handler_, &value);
    
    return value;
}