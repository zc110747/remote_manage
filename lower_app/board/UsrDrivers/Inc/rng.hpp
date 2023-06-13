//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     rng.hpp
//
//  Purpose:
//     rng driver interface.
//
// Author:
//      @zc
//
//  Assumptions:
//
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once");

#include "includes.hpp"

class rng_driver
{
public:
    BaseType_t init(void);

    static rng_driver *get_instance(void)
    {
        static rng_driver instance;
        return &instance;
    }
    
    uint32_t get_value(void);
    
private:
    RNG_HandleTypeDef rng_handler_;
};