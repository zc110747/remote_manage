//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//     SystemConfig.hpp
//
//  Purpose:
//     SystemConfig Interface.
//
// Author:
//      ZhangChao
//
//  Assumptions:
//
//  Revision History:
//      7/25/2022   Create New Version
//////////////////////////////////////////////////////////////////////////////
#ifndef _INCLUDE_SYSTEM_CONFIG_HPP
#define _INCLUDE_SYSTEM_CONFIG_HPP

#include "json/json.h"
#include "../include/productConfig.hpp"
#include <iostream>
#include "../logger/logger.hpp"

using JString = Json::String;

typedef struct 
{
    int init;
    JString dev;
}IoSysConfig;

typedef struct
{
    int baud;
    int dataBits;
    int stopBits;
    JString parity;
    JString dev;
}SerialSysConfig;

typedef struct 
{
    int port;
    JString ipaddr;
}SocketSysConfig;

typedef struct 
{
    JString dev;
}DeviceSysConfig;

typedef struct 
{
    //io config
    IoSysConfig led; 
    IoSysConfig beep;   

    //serial config
    SerialSysConfig serial;

    //socket config
    SocketSysConfig tcp;
    SocketSysConfig udp;
    SocketSysConfig logger;

    //deivce config
    DeviceSysConfig rtc;
    DeviceSysConfig icmSpi;
    DeviceSysConfig apI2c; 

    //path
    JString downloadpath;
}SystemParamter;

class SystemConfig
{
    friend std::ostream& operator<<(std::ostream&, const SystemConfig&);

private:
    static SystemConfig *pInstance;
    SystemParamter parameter;

public:
    SystemConfig() = default;
    ~SystemConfig() = delete;

    static SystemConfig* getInstance();
    bool init(const char* path);
    void default_init() noexcept;

public:
    const IoSysConfig *getled()         {return &(parameter.led);}
    const IoSysConfig *getbeep()        {return &(parameter.beep);}
    const SerialSysConfig *getserial()  {return &(parameter.serial);}
    const SocketSysConfig *gettcp()     {return &(parameter.tcp);}
    const SocketSysConfig *getudp()     {return &(parameter.udp);}
    const SocketSysConfig *getlogger()  {return &(parameter.logger);}
    const DeviceSysConfig *getrtc()     {return &(parameter.rtc);}
    const DeviceSysConfig *geticmSpi()  {return &(parameter.icmSpi);}
    const DeviceSysConfig *getapI2c()   {return &(parameter.apI2c);}
    const JString &getdownloadpath()    {return parameter.downloadpath;}
};
#endif
