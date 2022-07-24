//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      driver.cpp
//
//  Purpose:
//      driver init and release.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "driver.hpp"

bool hardware_driver_init()
{
    ledTheOne::getInstance()->open(O_RDWR | O_NDELAY);
    beepTheOne::getInstance()->open(O_RDWR | O_NDELAY);
    APDevice::getInstance()->open(O_RDONLY);
    ICMDevice::getInstance()->open(O_RDONLY);
    RTCDevice::getInstance()->open(O_RDONLY);
}

void hardware_driver_release()
{
    ledTheOne::getInstance()->release();
    beepTheOne::getInstance()->release();
    APDevice::getInstance()->release();
    ICMDevice::getInstance()->release();
    RTCDevice::getInstance()->release();
}
	