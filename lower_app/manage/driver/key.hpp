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

#include "deviceBase.hpp"

class KEY:public deviceBase
{
private:
    uint32_t keyvalue;
    
public:
    using deviceBase::deviceBase;

    //key初始化函数，输入文件描述符和支持flags
    virtual bool init(const std::string &DevicePath, int flags) override;

    //注册按键执行的回调事件，当按键触发后调用
    bool register_func(std::function<void(int)> func);
};