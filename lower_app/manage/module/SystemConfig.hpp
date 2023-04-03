//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SystemConfig.hpp
//
//  Purpose:
//      系统信息配置组件，主要实现如下功能
//      1.读取配置文件config.json, 赋值到内部配置中，如读取失败则使用默认配置
//      2.提供接口，允许上位机读取配置信息
//      3.提供接口，允许上位机写入配置信息，并保存到config.json文件中, 并支持一键复位
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "json/json.h"
#include "productConfig.hpp"
#include "logger.hpp"

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
    DeviceSysConfig key;

    //serial config
    SerialSysConfig serial;

    //socket config
    SocketSysConfig tcp;
    SocketSysConfig logger;
    SocketSysConfig node;
    SocketSysConfig web_node;

    //deivce config
    DeviceSysConfig rtc;
    DeviceSysConfig icmSpi;
    DeviceSysConfig apI2c; 

    //path
    JString downloadpath;
    uint8_t version[4];
}SystemParamter;

class SystemConfig
{
    friend std::ostream& operator<<(std::ostream&, const SystemConfig&);

private:
    static SystemConfig *pInstance;
    SystemParamter parameter;
    std::string filePath;

public:
    SystemConfig() = default;
    ~SystemConfig() = delete;

    static SystemConfig* getInstance();
    bool init(const char* path);
    void default_init() noexcept;
    void SaveConfigFile();             //保存Config的文件     

public:
    const std::string getFilePath() const    {return filePath;};
    const IoSysConfig *getled()             {return &(parameter.led);}
    const IoSysConfig *getbeep()            {return &(parameter.beep);}
    const DeviceSysConfig *getkey()         {return &(parameter.key);}
    const SerialSysConfig *getserial()      {return &(parameter.serial);}
    const SocketSysConfig *gettcp()         {return &(parameter.tcp);}
    const SocketSysConfig *getlogger()      {return &(parameter.logger);}
    const SocketSysConfig *getnode()        {return &(parameter.node);}
    const DeviceSysConfig *getrtc()         {return &(parameter.rtc);}
    const DeviceSysConfig *geticmSpi()      {return &(parameter.icmSpi);}
    const DeviceSysConfig *getapI2c()       {return &(parameter.apI2c);}
    const JString &getdownloadpath()        {return parameter.downloadpath;}
    const uint8_t* getversion()             {return parameter.version;}
};

const uint8_t *get_version();
