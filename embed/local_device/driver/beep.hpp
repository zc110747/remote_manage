//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      beep.hpp
//
//  Purpose:
//      蜂鸣器管理驱动，包含IO支持特性
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

class beep_device:public io_base
{
public:
    /// \brief constructor
    using io_base::io_base;
};
