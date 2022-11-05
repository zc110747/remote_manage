//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     beep.hpp
//
//  Purpose:
//      Device Beep Interface.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
//////////////////////////////////////////////////////////////////////////////
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
