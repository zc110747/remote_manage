/*
 * File      : device.hpp
 * 
 * device interface for all
 * 
 * COPYRIGHT (C) 2022, zc
 */

#ifndef __DEVICEBASE_HPP
#define __DEVICEBASE_HPP

#include "../include/productConfig.hpp"

class deviceBase
{
private:
    std::string devicePathM;

protected:
    int DeviceFdM;

public:
    deviceBase();
    deviceBase(const std::string &DevicePath);
        ~deviceBase();
    bool open(int flags);
    void close();

public:
    int getfd() { return DeviceFdM; }
    const std::string &getDevicePath() {return devicePathM;}
};

class IoBase:public deviceBase
{
public:
    //constructor
    using deviceBase::deviceBase;

    //io read/write
    int readIoStatus();
    bool writeIoStatus(uint8_t status);
};

#endif
