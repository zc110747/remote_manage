/*
 * File      : led.cpp
 * Led应用层驱动实现
 * COPYRIGHT (C) 2020, zc
 */
#include "led.hpp"

led* led::pInstance = nullptr;
led::led(const std::string &path)
:deviceBase(path)
{

}

led* led::getInstance()
{
    if(pInstance == nullptr)
    {
        struct SSystemConfig *pSystemConfigInfo;
        pSystemConfigInfo = GetSSytemConfigInfo();
        pInstance = new(std::nothrow) led(pSystemConfigInfo->m_dev_led);
        if(pInstance == nullptr)
        {
            //do something
        }
    }
    return pInstance;
}

int led::readIoStatus()
{
    uint8_t nValue = -1;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = read(DeviceFdM, &nValue, 1);  //将数据写入LED
        if(nSize <= 0)
        {
            //do read error process
        }
    }
    return nValue;
}

bool led::writeIoStatus(uint8_t status)
{
    bool ret = false;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = write(DeviceFdM, &status, 1);  //将数据写入LED
        if(nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}