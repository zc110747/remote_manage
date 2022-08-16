//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      led.hpp
//
//  Purpose:
//      led device interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_LED_HPP
#define _INCLUDE_LED_HPP

#include "deviceBase.hpp"

class ledTheOne: public IoBase
{
private:
    static ledTheOne *pInstance;

public:
    using IoBase::IoBase;

    static ledTheOne* getInstance();
    void release();

    void test();
};
#endif
