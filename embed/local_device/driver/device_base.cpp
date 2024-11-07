//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_base.cpp
//
//  Purpose:
//      封装的设备底层接口，基类为device_base, 支持open/close
//      io_base主要封装IO支持的接口, open, read, write, close
//      info_base主要封装支持特定外设的接口, 可以读取内部信息
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
    device_path_.clear();
    device_fd_ = -1;
}

device_base::~device_base()
{
    device_path_.clear();
    close();
}

bool device_base::open(int flags)
{
    device_fd_ = ::open(device_path_.c_str(), flags);
    if (device_fd_ == -1)
    {
        PRINT_LOG(LOG_INFO, 0, "open %s device failed!", device_path_.c_str());
        return false;
    }

    PRINT_LOG(LOG_INFO, 0, "open %s device success, fd:%d!", device_path_.c_str(), device_fd_);
    return true;
}

bool device_base::init(const std::string &DevicePath, int flags)
{
    device_path_ = DevicePath;
    return open(flags);
}

void device_base::close()
{
    if (device_fd_ != -1)
    {
        device_fd_ = -1;
        ::close(device_fd_);
    }
}

bool io_base::read_io_status()
{
    bool ret = false;
    ssize_t nSize;

    if (device_fd_ >= 0)
    {
        nSize = ::read(device_fd_, &status_, 1);  //将数据写入ledBase
        if (nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}

bool io_base::write_io_status(uint8_t status) 
{
    bool ret = false;
    ssize_t nSize;

    if (device_fd_ >= 0)
    {
        nSize = ::write(device_fd_, &status, 1);  //将数据写入ledBase
        if (nSize > 0)
        {
            ret = true;
        }
    }
    return ret;
}

bool io_base::on()
{
    return write_io_status(1);
}

bool io_base::off()
{
    return write_io_status(0);
}

bool io_base::trigger()
{
    bool ret = false;
    
    if (read_io_status())
    {
        if (status_)
            ret = off();
        else
            ret = on();
    }
    return ret;
}
