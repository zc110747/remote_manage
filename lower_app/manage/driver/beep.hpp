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
#ifndef _INCLUDE_BEEP_H
#define _INCLUDE_BEEP_H

#include "deviceBase.hpp"

class beepTheOne:public IoBase
{
private:
    static beepTheOne* pInstance;

public:
    //consturctor
    using IoBase::IoBase;

    static beepTheOne* getInstance();
    void release();
};

#endif
