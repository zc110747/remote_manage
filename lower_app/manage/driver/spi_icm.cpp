//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      spi_icm.cpp
//
//  Purpose:
//      ICM模块对应的SPI接口，通过继承支持直接读取模块信息
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
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
