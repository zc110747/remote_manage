//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      rtc.hpp
//
//  Purpose:
//      rtc时钟, 支持访问本地和硬件时钟
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
#include <linux/rtc.h>

class rtc_device:public device_base
{
public:
    /// \brief constructor
    using device_base::device_base;

    /// \brief init
    /// - This method is used to init the object.
    /// \param DevicePath - path of the system device.
    /// \param flags - flags process the device.
    /// \return Wheather initialize is success or failed.
    bool init(const std::string &DevicePath, int flags);

    /// \brief update_rtc_time
    /// - This method is used to update the rtc time.
    /// \return Wheather time update is success or failed.
    bool update_rtc_time();

    /// \brief get_current_time
    /// - This method is used to get current time from start.
    /// \return The time from the start.
    int get_current_time();
    
private:
    /// \brief rtc_time_
    /// - rtc time struct data.
    struct rtc_time rtc_time_{0};

    /// \brief start_time_
    /// - the time when init, used to calculate seconds.
    uint64_t start_time_{0};
};

