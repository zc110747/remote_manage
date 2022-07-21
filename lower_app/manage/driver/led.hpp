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
#include "../include/SystemConfig.h"

class led:public deviceBase
{
private:

public:
    led(const std::string &path);

    static led *pInstance;
    static led *getInstance();

    int readIoStatus();
    bool writeIoStatus(uint8_t status);
};
#endif
