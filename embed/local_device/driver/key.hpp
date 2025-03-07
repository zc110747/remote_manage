//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      key.hpp
//
//  Purpose:
//      按键管理模块,处理按键的初始化和注册回调事件
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "device_base.hpp"

#define KEY_NUM     11
#define KEY_PREES   1
#define KEY_RELEASE 0

class key_device:public device_base
{
public:
    /// \brief constructor
    using device_base::device_base;

    /// \brief init
    /// - This method is used to init the object.
    /// \param DevicePath - path of the system device.
    /// \param flags - flags process the device.
    /// \return Wheather initialize is success or failed.
    virtual bool init(const std::string &DevicePath, int flags) override;

    /// \brief register_func
    /// - This method is used to register function for key callback.
    /// \param func - function register for the key.
    /// \return Wheather register is success or failed.
    bool register_func(uint16_t key_num, uint16_t key_event, std::function<void(uint16_t, uint16_t)> func);

private:
    /// \brief run
    /// - This method is used for thread run key.
    void run();
};