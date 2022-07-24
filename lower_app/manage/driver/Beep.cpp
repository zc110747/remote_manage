//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      beep.cpp
//
//  Purpose:
//      Device Beep.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "beep.hpp"
#include "../include/SystemConfig.h"

beepTheOne* beepTheOne::pInstance = nullptr;
beepTheOne* beepTheOne::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) beepTheOne(static_cast<SSystemConfig *>(GetSSytemConfigInfo())->m_dev_beep);
        if(pInstance == NULL)
        {
            //To Do something(may logger)
        }
    }
    return pInstance;
}

void beepTheOne::release()
{
    if(pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}