//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      device_base.cpp
//
//  Purpose:
//      封装的设备底层接口，基类为device_base, 支持open/close
//      IOBase主要封装IO支持的接口, open, read, write, close
//      InfoBase主要封装支持特定外设的接口, 可以读取内部信息
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
#include "device_base.hpp"

using std::string;

device_base::device_base()
{
    devicePathM.clear();
    DeviceFdM = -1;
}

device_base::device_base(const string &DevicePath)
{
    devicePathM = DevicePath;
    DeviceFdM = -1;
}

device_base::~device_base()
{
    devicePathM.clear();
    close();
}

bool device_base::open(int flags)
{
    DeviceFdM = ::open(devicePathM.c_str(), flags);
    if(DeviceFdM == -1)
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device failed!", devicePathM.c_str());
        return false;
    }

    PRINT_LOG(LOG_INFO, 0, "open %s device success, fd:%d!", devicePathM.c_str(), DeviceFdM);
    return true;
}

bool device_base::init(const std::string &DevicePath, int flags)
{
    devicePathM = DevicePath;
    return open(flags);
}

void device_base::close()
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

bool IoBase::on()
{
    return writeIoStatus(1);
}

bool IoBase::off()
{
    return writeIoStatus(0);
}

bool IoBase::trigger()
{
    bool ret = false;
    
    if(readIoStatus())
    {
        if(IoStatus)
            ret = off();
        else
            ret = on();
    }
    return ret;
}