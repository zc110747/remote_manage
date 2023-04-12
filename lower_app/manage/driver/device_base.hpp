//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      device_base.hpp
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
#ifndef __device_base_HPP
#define __device_base_HPP

#include "../include/productConfig.hpp"
#include "system_config.hpp"

class device_base
{
protected:
    int device_fd_;
    std::string device_path_;

public:
    device_base();
    device_base(const std::string &DevicePath);
        ~device_base();
    virtual bool open(int flags);
    virtual void close();
    virtual bool init(const std::string &DevicePath, int flags);

public:
    int getfd() { return device_fd_; }
    const std::string &getDevicePath() {return device_path_;}
};

class io_base:public device_base
{
private:
    std::atomic<uint8_t> status_{0};

public:
    //constructor
    using device_base::device_base;

    //io read/write
    bool readIoStatus();
    bool writeIoStatus(uint8_t status);
    bool on();
    bool off();
    bool trigger();
    
    uint8_t getIoStatus()  {return status_;}
};

#define MAX_INFO_SIZE   48
template<typename T>
class info_base:public device_base
{
protected:
    union{
        char buffer[MAX_INFO_SIZE];
        T info;
    }data;

public:
    //constructor
    using device_base::device_base;

    bool readInfo()
    {
        bool ret = false;
        ssize_t nSize;

        if(device_fd_ >= 0)
        {
            int readSize = sizeof(data.info);
            
            readSize = readSize<MAX_INFO_SIZE?readSize:MAX_INFO_SIZE;
            nSize = ::read(device_fd_, data.buffer, readSize);
            if(nSize > 0)
            {
                ret = true;
            }
            else
            {

            }
        }
        return ret;
    }

    T getInfo() {return data.info;}
};

#endif
