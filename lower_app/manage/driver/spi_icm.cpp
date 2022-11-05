//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi_icm.cpp
//
//  Purpose:
//      spi read icm application.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "spi_icm.hpp"

ICMDevice* ICMDevice::pInstance = nullptr;
ICMDevice* ICMDevice::getInstance()
{
    if(pInstance == nullptr)
    {
       pInstance = new(std::nothrow) ICMDevice(SystemConfig::getInstance()->geticmSpi()->dev); 
    }
    return pInstance;
}

void ICMDevice::release()
{
    if(pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
}
