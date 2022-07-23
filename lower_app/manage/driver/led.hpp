/*
 * File      : led.hpp
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
};
#endif
