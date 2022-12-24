//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      key.hpp
//
//  Purpose:
//      KEY管理驱动，输入的硬件特性
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

#include "deviceBase.hpp"

class KEY:public deviceBase
{
private:
    uint32_t keyvalue;
    
public:
    using deviceBase::deviceBase;
    virtual bool init(const std::string &DevicePath, int flags) override;
    bool register_func(std::function<void(int)> func);
};