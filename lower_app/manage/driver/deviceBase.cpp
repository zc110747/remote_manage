//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      deviceBase.cpp
//
//  Purpose:
//      Device Base Class.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
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

bool IoBase::readIoStatus()
{
    bool ret = false;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = ::read(DeviceFdM, &IoStatus, 1);  //将数据写入ledBase
        if(nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}

bool IoBase::writeIoStatus(uint8_t status)
{
    bool ret = false;
    ssize_t nSize;

    if(DeviceFdM >= 0)
    {
        nSize = ::write(DeviceFdM, &status, 1);  //将数据写入ledBase
        if(nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}
