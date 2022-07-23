/*
 * File      : beep.cpp
 * beep应用层驱动实现
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/

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