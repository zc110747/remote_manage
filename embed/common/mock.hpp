//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mock.hpp
//
//  Purpose:
//      用于获取随机数据
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"

class Mock
{
public:
    /// \brief constructor
    Mock() = default;

    /// \brief srand_init
    /// - This method is used to init srand.
    void srand_init(void)
    {
        is_srand_init = true;
    }

    /// \brief get_random_int
    /// - This method is used to get random int data.
    /// \param min - the min int value for the random.
    /// \param max - the max int value for the random.
    /// \return the random int value.
    int get_random_int(uint32_t min, uint32_t max)
    {
        int range;
        uint32_t seed = (unsigned)time(NULL);

        if (max <= min || !is_srand_init)
            return max;
        
        range  = max - min + 1;

        return (min + rand_r(&seed)%range);
    }

    /// \brief get_random_float
    /// - This method is used to get random float data.
    /// \param min - the min float value for the random.
    /// \param max - the max float value for the random.
    /// \return the random float value.
    float get_random_float(float min, float max)
    { 
        int min_i, max_i;
        int range, out_i;
        float out_f;
        uint32_t seed = (unsigned)time(NULL);

        if (max <= min || !is_srand_init)
            return max;
        
        min_i = (int)min*100;
        max_i = (int)max*100;
        range  = max_i - min_i + 1;
        out_i = min_i + rand_r(&seed)%range;
        out_f = ((float)out_i)/100;

        return out_f;
    }

private:
    /// \brief is_srand_init
    /// - defined parameter wheather already init.
    bool is_srand_init{false};
};
