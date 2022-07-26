/*
 * File      : SystemConfig.cpp
 * 系统配置信息文件处理
 * COPYRIGHT (C) 2020, zc
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-5-4      zc           the first version
 */

/**
 * @addtogroup IMX6ULL
 */
/*@{*/
#include <fstream>
#include <iostream>
#include "SystemConfig.hpp"


SystemConfig::SystemConfig()
{

}

SystemConfig::~SystemConfig()
{

}

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

bool SystemConfig::init(const char* path)
{
    Json::Value root;
    std::ifstream ifs;
    ifs.open(path);

    Json::CharReaderBuilder builder;
    builder["collectComments"] = true;
    JSONCPP_STRING errs;
    if (!parseFromStream(builder, ifs, &root, &errs)) {
        std::cout << errs << std::endl;
        default_init();
        return false;
    }
    //std::cout << root << std::endl;

    try
    {
        parameter.led.init = root["led"]["init"].asInt();
        parameter.led.dev = root["led"]["dev"].asString();

        parameter.beep.init = root["beep"]["init"].asInt();
        parameter.beep.dev = root["beep"]["dev"].asString();

        parameter.serial.baud = root["serial"]["baud"].asInt();
        parameter.serial.dataBits = root["serial"]["dataBits"].asInt();;
        parameter.serial.stopBits = root["serial"]["stopBits"].asInt();;
        parameter.serial.parity = root["serial"]["parity"].asString();;
        parameter.serial.dev = root["serial"]["dev"].asString();

        parameter.tcp.ipaddr = root["tcp"]["ipaddr"].asString();
        parameter.tcp.port = root["tcp"]["port"].asInt();
        parameter.udp.ipaddr = root["udp"]["ipaddr"].asString();
        parameter.udp.port = root["udp"]["port"].asInt();

        parameter.rtc.dev = root["rtc"]["dev"].asString();
        parameter.icmSpi.dev = root["icmSpi"]["dev"].asString();; 
        parameter.apI2c.dev = root["apI2c"]["dev"].asString();;

        parameter.downloadpath = root["downloadpath"].asString();
    }
    catch(const std::exception& e)
    {
        default_init();
        ifs.close();
        std::cerr << e.what() << '\n';
        return false;
    }
    
    ifs.close();
    return true;
}

void SystemConfig::default_init()
{
    parameter.led.init = DEFAULT_LED_INIT;
    parameter.led.dev = DEFALUT_LED_DEV;

    parameter.beep.init = DEFAULT_BEEP_INIT;
    parameter.beep.dev = DEFALUT_BEEP_DEV;

    parameter.serial.baud = DEFAULT_SERIAL_BAUD;
    parameter.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter.serial.parity = DEFAULT_SERIAL_PARITY;
    parameter.serial.dev = DEFAULT_SERIAL_DEV;

    parameter.tcp.ipaddr = DEFAULT_TCP_IPADDR;
    parameter.tcp.port = DEFAULT_TCP_PORT;
    parameter.udp.ipaddr = DEFAULT_UDP_IPADDR;
    parameter.udp.port = DEFAULT_UDP_PORT;

    parameter.rtc.dev = DEFAULT_RTC_DEV;
    parameter.icmSpi.dev = DEFAULT_ICMSPI_DEV; 
    parameter.apI2c.dev = DEFAULT_API2C_DEV;

    parameter.downloadpath = DEFAULT_DOWNLOAD_PATH;
}

std::ostream& operator<<(std::ostream& os, const SystemConfig& config)
{
    const SystemParamter* parameter = &(config.parameter);

    os<<"led:"<<parameter->led.dev<<" "<<parameter->led.init<<"\n";
    os<<"beep:"<<parameter->beep.dev<<" "<<parameter->beep.init<<"\n";
    
    os<<"serial:"<<parameter->serial.dev<<" "<<parameter->serial.baud<<" "
    <<parameter->serial.dataBits<<parameter->serial.stopBits<<" "
    <<parameter->serial.parity<<"\n";

    os<<"tcp:"<<parameter->tcp.ipaddr<<" "<<parameter->tcp.port<<"\n";
    os<<"udp:"<<parameter->udp.ipaddr<<" "<<parameter->udp.port<<"\n";
    os<<"rtc:"<<parameter->rtc.dev<<"\n";
    os<<"icmSpi:"<<parameter->icmSpi.dev<<"\n";
    os<<"apI2c:"<<parameter->apI2c.dev<<"\n";
    os<<"downloadpath:"<<parameter->downloadpath;
    return os;
}
