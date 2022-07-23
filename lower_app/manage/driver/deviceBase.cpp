/*
 * File      : device.cpp
 * 
 * device interface for all
 * 
 * COPYRIGHT (C) 2022, zc
 */

#include "deviceBase.hpp"

using std::string;

deviceBase::deviceBase()
{
    devicePathM.clear();
    DeviceFdM = -1;
}

deviceBase::deviceBase(const string &DevicePath)
{
    devicePathM = DevicePath;
    DeviceFdM = -1;
}

deviceBase::~deviceBase()
{
    devicePathM.clear();
    close();
}

bool deviceBase::open(int flags)
{
    DeviceFdM = ::open(devicePathM.c_str(), flags);
    if(DeviceFdM == -1)
    {
        //do error process
        return false;
    }
    return true;
}

void deviceBase::close()
{
    if(DeviceFdM != 1)
    {
        DeviceFdM = -1;
        ::close(DeviceFdM);
    }
}

int IoBase::readIoStatus()
{
    uint8_t nValue = -1;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = read(DeviceFdM, &nValue, 1);  //将数据写入ledBase
        if(nSize <= 0)
        {
            //do read error process
        }
    }
    return nValue;
}

bool IoBase::writeIoStatus(uint8_t status)
{
    bool ret = false;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = write(DeviceFdM, &status, 1);  //将数据写入ledBase
        if(nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}