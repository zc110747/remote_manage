//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver.hpp
//
//  Purpose:
//      driver init and release interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include "led.hpp"
#include "beep.hpp"
#include "i2c_ap.hpp"
#include "spi_icm.hpp"
#include "rtc.hpp"

bool hardware_driver_init();
void hardware_driver_release();
#endif
