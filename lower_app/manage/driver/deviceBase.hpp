/*
 * File      : device.hpp
 * 
 * device interface for all
 * 
 * COPYRIGHT (C) 2022, zc
 */

#include <string>
#include "../include/UsrTypeDef.h"

class deviceBase
{
private:
    std::string devicePathM;

protected:
    int DeviceFdM;

public:
    deviceBase(const std::string &DevicePath);
        ~deviceBase();
    bool open(int flags);
    void close();
    int getfd() { return DeviceFdM; }
    const std::string &getDevicePath() {return devicePathM;}
};