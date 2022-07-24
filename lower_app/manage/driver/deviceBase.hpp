//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      deviceBase.hpp
//
//  Purpose:
//      Device Base Class Interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/24/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

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
    virtual bool open(int flags);
    virtual void close();

public:
    int getfd() { return DeviceFdM; }
    const std::string &getDevicePath() {return devicePathM;}
};

class IoBase:public deviceBase
{
private:
    uint8_t IoStatus{0};

public:
    //constructor
    using deviceBase::deviceBase;

    //io read/write
    bool readIoStatus();
    bool writeIoStatus(uint8_t status);

    uint8_t getIoStatus()  {return IoStatus;}
};

#define MAX_INFO_SIZE   48
template<typename T>
class InfoBase:public deviceBase
{
private:
    union{
        char buffer[MAX_INFO_SIZE];
        T info;
    }data;

public:
    //constructor
    using deviceBase::deviceBase;

    bool readInfo();
    T* getInfo() {return &(data.info);}
};

template<typename T>
bool InfoBase<T>::readInfo()
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

#endif
