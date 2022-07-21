/*
 * File      : device.cpp
 * 
 * device interface for all
 * 
 * COPYRIGHT (C) 2022, zc
 */

#include "deviceBase.hpp"

using std::string;
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