//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      spi_icm.hpp
//
//  Purpose:
//      ICM模块对应的SPI接口，通过继承支持直接读取模块信息
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

#include "device_base.hpp"

class icm_device : public info_base<icm_adc_info>
{
public:
    /// \brief constructor
    using info_base::info_base;

    /// \brief calculate_angle
    /// - This method is used to calculate the angle.
    void calculate_angle(void);

    /// \brief get_icm_info
    /// - This method is used to get icm info.
    /// return current icm info.
    const icm_info& get_icm_info()   { return icm_info_; }

    /// \brief get_icm_info
    /// - This method is used to get icm info.
    int32_t get_angle()          { return angle_; }

private:
    /// \brief icm_info_
    /// - icm device read information.
    icm_info icm_info_;

    /// \brief angle_
    /// - angle data calculate from icm info.
    int32_t angle_{0};
};

