//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SystemConfig.cpp
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
#include <fstream>
#include <iostream>
#include <memory>
#include "SystemConfig.hpp"

//const默认内部链接的, 需要在初始化地方加上const
const uint8_t fw_version[4] = {
    #include "../verion.txt"
};
SystemConfig* SystemConfig::pInstance = nullptr;
SystemConfig* SystemConfig::getInstance()
{
    if(pInstance == nullptr)
    {
        pInstance = new(std::nothrow) SystemConfig();
        if(pInstance == nullptr)
        {
            //to do error process
        }
    }
    return pInstance;
}

[[nodiscard]]
bool SystemConfig::init(const char* path)
{
    Json::Value root;
    std::ifstream ifs;

    filePath.clear();
    filePath.assign(path);

    //before init, need default init
    default_init();

    ifs.open(path);
    if(!ifs.is_open())
    {
        return false;
    }

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        return false;
    }
    //std::cout << root << std::endl;

    try
    {
        parameter.led.init = root["led"]["init"].asInt();
        parameter.led.dev = root["led"]["dev"].asString();

        parameter.beep.init = root["beep"]["init"].asInt();
        parameter.beep.dev = root["beep"]["dev"].asString();

        parameter.key.dev = root["key"]["dev"].asString();

        parameter.serial.baud = root["serial"]["baud"].asInt();
        parameter.serial.dataBits = root["serial"]["dataBits"].asInt();;
        parameter.serial.stopBits = root["serial"]["stopBits"].asInt();;
        parameter.serial.parity = root["serial"]["parity"].asString();;
        parameter.serial.dev = root["serial"]["dev"].asString();

        parameter.tcp.ipaddr = root["socket"]["ipaddr"].asString();
        parameter.tcp.port = root["tcp"]["port"].asInt();
        parameter.logger.ipaddr = root["socket"]["ipaddr"].asString();
        parameter.logger.port = root["logger"]["port"].asInt();
        parameter.node.ipaddr = LOCAL_HOST;
        parameter.node.port = root["node"]["socket_port"].asInt();

        parameter.rtc.dev = root["rtc"]["dev"].asString();
        parameter.icmSpi.dev = root["icmSpi"]["dev"].asString();; 
        parameter.apI2c.dev = root["apI2c"]["dev"].asString();;

        parameter.downloadpath = root["downloadpath"].asString();
    }
    catch(const std::exception& e)
    {
        ifs.close();
        std::cerr << e.what() << '\n';
        return false;
    }
    
    ifs.close();
    return true;
}

void SystemConfig::default_init() noexcept
{
    parameter.led.init = DEFAULT_LED_INIT;
    parameter.led.dev = DEFAULT_LED_DEV;

    parameter.beep.init = DEFAULT_BEEP_INIT;
    parameter.beep.dev = DEFAULT_BEEP_DEV;

    parameter.key.dev = DEFAULT_KEY_DEV;

    parameter.serial.baud = DEFAULT_SERIAL_BAUD;
    parameter.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter.serial.parity = DEFAULT_SERIAL_PARITY;
    parameter.serial.dev = DEFAULT_SERIAL_DEV;

    parameter.tcp.ipaddr = DEFAULT_TCP_IPADDR;
    parameter.tcp.port = DEFAULT_TCP_PORT;
    parameter.logger.ipaddr = DEFAULT_LOGGER_IPADDR;
    parameter.logger.port = DEFAULT_LOGGER_PORT;
    parameter.node.ipaddr = LOCAL_HOST;
    parameter.node.port = DEFAULT_NODE_PORT;
    parameter.web_node.ipaddr = LOCAL_HOST;
    parameter.web_node.port = DEFAULT_NODE_WEB_PORT;

    parameter.rtc.dev = DEFAULT_RTC_DEV;
    parameter.icmSpi.dev = DEFAULT_ICMSPI_DEV; 
    parameter.apI2c.dev = DEFAULT_API2C_DEV;

    parameter.downloadpath = DEFAULT_DOWNLOAD_PATH;
    memcpy(parameter.version, fw_version, 4);
}

void SystemConfig::SaveConfigFile()
{
    Json::Value root;

    //gpio
    root["led"]["dev"] = parameter.led.dev;
    root["led"]["init"] = parameter.led.init;
    root["beep"]["dev"] = parameter.beep.dev;
    root["beep"]["init"] = parameter.beep.init;
    root["key"]["dev"] = parameter.key.dev;

    root["serial"]["baud"] = parameter.serial.baud;
    root["serial"]["dataBits"] = parameter.serial.dataBits;
    root["serial"]["StopBits"] = parameter.serial.stopBits;
    root["serial"]["parity"] = parameter.serial.parity;
    root["serial"]["dev"] = parameter.serial.dev;

    root["socket"]["ipaddr"] = parameter.tcp.ipaddr;
    root["tcp"]["port"] = parameter.tcp.port;
    root["logger"]["port"] = parameter.logger.port;

    root["node"]["socket_port"] = parameter.node.port;
    root["node"]["web_port"] = parameter.web_node.port;
    root["rtc"]["dev"] = parameter.rtc.dev;
    root["icmSpi"]["dev"] = parameter.icmSpi.dev;
    root["apI2c"]["dev"] = parameter.apI2c.dev;
    root["downloadpath"] = parameter.downloadpath;

    Json::StreamWriterBuilder swb;
    std::shared_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    std::stringstream str;
    sw->write(root, &str);
    auto s = str.str();
    
    std::ofstream fstr;
    fstr.open(filePath.c_str(), std::ios::out);
    fstr.write(s.c_str(), s.size());
    fstr.close();
}

std::ostream& operator<<(std::ostream& os, const SystemConfig& config)
{
    const SystemParamter* parameter = &(config.parameter);

    os<<"filepath:"<<config.getFilePath()<<"\n";
    os<<"led:"<<parameter->led.dev<<" "<<parameter->led.init<<"\n";
    os<<"beep:"<<parameter->beep.dev<<" "<<parameter->beep.init<<"\n";
    
    os<<"serial:"<<parameter->serial.dev<<" "<<parameter->serial.baud<<" "
    <<parameter->serial.dataBits<<parameter->serial.stopBits<<" "
    <<parameter->serial.parity<<"\n";

    os<<"tcp:"<<parameter->tcp.ipaddr<<" "<<parameter->tcp.port<<"\n";
    os<<"logger:"<<parameter->logger.ipaddr<<" "<<parameter->logger.port<<"\n";
    os<<"node:"<<parameter->node.ipaddr<<" "<<parameter->node.port<<"\n";
    os<<"rtc:"<<parameter->rtc.dev<<"\n";
    os<<"icmSpi:"<<parameter->icmSpi.dev<<"\n";
    os<<"apI2c:"<<parameter->apI2c.dev<<"\n";
    os<<"downloadpath:"<<parameter->downloadpath;
    return os;
}

const uint8_t *get_version()
{
    return fw_version;
}