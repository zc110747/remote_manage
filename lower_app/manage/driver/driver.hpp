
/*
 * File      : driver.hpp
 * 驱动实现接口
 * COPYRIGHT (C) 2020, zc
 */
#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include "led.hpp"
#include "beep.hpp"

bool hardware_driver_init();
void hardware_driver_release();
#endif
