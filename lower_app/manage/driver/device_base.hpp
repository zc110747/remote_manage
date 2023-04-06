//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      device_base.hpp
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
#ifndef __device_base_HPP
#define __device_base_HPP

#include "../include/productConfig.hpp"
#include "system_config.hpp"

class device_base
{
protected:
    int DeviceFdM;
    std::string devicePathM;

public:
    device_base();
    device_base(const std::string &DevicePath);
        ~device_base();
    virtual bool open(int flags);
    virtual void close();
    virtual bool init(const std::string &DevicePath, int flags);

public:
    int getfd() { return DeviceFdM; }
    const std::string &getDevicePath() {return devicePathM;}
};

class IoBase:public device_base
{
private:
    std::atomic<uint8_t> IoStatus{0};

public:
    //constructor
    using device_base::device_base;

    //io read/write
    bool readIoStatus();
    bool writeIoStatus(uint8_t status);
    bool on();
    bool off();
    bool trigger();
    
    uint8_t getIoStatus()  {return IoStatus;}
};

#define MAX_INFO_SIZE   48
template<typename T>
class InfoBase:public device_base
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

        if(DeviceFdM >= 0)
        {
            int readSize = sizeof(data.info);
            
            readSize = readSize<MAX_INFO_SIZE?readSize:MAX_INFO_SIZE;
            nSize = ::read(DeviceFdM, data.buffer, readSize);
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
