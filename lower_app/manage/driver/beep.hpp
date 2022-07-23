/*
 * File      : beep.h
 * beep应用层驱动实现接口
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
#ifndef _INCLUDE_BEEP_H
#define _INCLUDE_BEEP_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "deviceBase.hpp"

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

/**************************************************************************
* Global Functon Declaration
***************************************************************************/
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
