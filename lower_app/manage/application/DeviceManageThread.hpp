//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      WorkflowThread.hpp
//
//  Purpose:
//      Deivce Application Process interface.
//
// Author:
//     	Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/30/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_APP_TASK_H
#define _INCLUDE_APP_TASK_H

#include "tools/FifoManage.hpp"
#include "tools/MqManage.hpp"
#include "driver.hpp"
#include <type_traits>
#include <cstring>
#include <mutex>
#include <chrono>

struct DeviceReadInfo
{
    uint8_t   led_io;
    uint8_t   beep_io; 
    AP_INFO   ap_info;
    ICM_INFO  icm_info;

    //for compare, need clear before to avoid fill by align.
    bool operator != (const DeviceReadInfo& dev_info)
    {
        static_assert(std::is_trivial_v<DeviceReadInfo>, "Not Allow C memory process!");

        if(memcmp((char *)this, (char *)&dev_info, size()) != 0)
            return true;
        return false;
    }

    void clear()
    {
        static_assert(std::is_trivial_v<DeviceReadInfo>, "Not Allow C memory process!");

        memset((char *)this, 0, size());
    }

    size_t size()
    {
        return sizeof(*this);
    }
};

class DeviceManageThread
{
private:
    DeviceReadInfo inter_info;
    DeviceReadInfo outer_info;
    std::mutex mut;
    std::thread dmt_thread;
    static DeviceManageThread* pInstance;

    void run();
    void update();

public:
    DeviceManageThread() = default;
    virtual ~DeviceManageThread() = default; 

    bool init();
    DeviceReadInfo getDeviceInfo();
    
    static DeviceManageThread* getInstance();
};

#endif
