//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      system_config.cpp
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
#include "system_config.hpp"

//const默认内部链接的, 需要在初始化地方加上const
const uint8_t fw_version[4] = {
    #include "../verion.txt"
};
system_config* system_config::instance_pointer_ = nullptr;
system_config* system_config::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) system_config();
        if(instance_pointer_ == nullptr)
        {
            //to do error process
        }
    }
    return instance_pointer_;
}

[[nodiscard]]
bool system_config::init(const char* path)
{
    Json::Value root;
    std::ifstream ifs;

    file_path_.clear();
    file_path_.assign(path);

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
        parameter_.led.init = root["led"]["init"].asInt();
        parameter_.led.dev = root["led"]["dev"].asString();

        parameter_.beep.init = root["beep"]["init"].asInt();
        parameter_.beep.dev = root["beep"]["dev"].asString();

        parameter_.key.dev = root["key"]["dev"].asString();

        parameter_.serial.baud = root["serial"]["baud"].asInt();
        parameter_.serial.dataBits = root["serial"]["dataBits"].asInt();;
        parameter_.serial.stopBits = root["serial"]["stopBits"].asInt();;
        parameter_.serial.parity = root["serial"]["parity"].asString();;
        parameter_.serial.dev = root["serial"]["dev"].asString();

        parameter_.tcp.ipaddr = root["socket"]["ipaddr"].asString();
        parameter_.tcp.port = root["tcp"]["port"].asInt();
        parameter_.logger.ipaddr = root["socket"]["ipaddr"].asString();
        parameter_.logger.port = root["logger"]["port"].asInt();
        parameter_.node.ipaddr = LOCAL_HOST;
        parameter_.node.port = root["node"]["socket_port"].asInt();

        parameter_.rtc.dev = root["rtc"]["dev"].asString();
        parameter_.icmSpi.dev = root["icmSpi"]["dev"].asString();; 
        parameter_.apI2c.dev = root["apI2c"]["dev"].asString();;

        parameter_.downloadpath = root["downloadpath"].asString();
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

void system_config::default_init() noexcept
{
    parameter_.led.init = DEFAULT_LED_INIT;
    parameter_.led.dev = DEFAULT_LED_DEV;

    parameter_.beep.init = DEFAULT_BEEP_INIT;
    parameter_.beep.dev = DEFAULT_BEEP_DEV;

    parameter_.key.dev = DEFAULT_KEY_DEV;

    parameter_.serial.baud = DEFAULT_SERIAL_BAUD;
    parameter_.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter_.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter_.serial.parity = DEFAULT_SERIAL_PARITY;
    parameter_.serial.dev = DEFAULT_SERIAL_DEV;

    parameter_.tcp.ipaddr = DEFAULT_TCP_IPADDR;
    parameter_.tcp.port = DEFAULT_TCP_PORT;
    parameter_.logger.ipaddr = DEFAULT_LOGGER_IPADDR;
    parameter_.logger.port = DEFAULT_LOGGER_PORT;
    parameter_.node.ipaddr = LOCAL_HOST;
    parameter_.node.port = DEFAULT_NODE_PORT;
    parameter_.web_node.ipaddr = LOCAL_HOST;
    parameter_.web_node.port = DEFAULT_NODE_WEB_PORT;

    parameter_.rtc.dev = DEFAULT_RTC_DEV;
    parameter_.icmSpi.dev = DEFAULT_ICMSPI_DEV; 
    parameter_.apI2c.dev = DEFAULT_API2C_DEV;

    parameter_.downloadpath = DEFAULT_DOWNLOAD_PATH;
    memcpy(parameter_.version, fw_version, 4);
}

void system_config::SaveConfigFile()
{
    Json::Value root;

    //gpio
    root["led"]["dev"] = parameter_.led.dev;
    root["led"]["init"] = parameter_.led.init;
    root["beep"]["dev"] = parameter_.beep.dev;
    root["beep"]["init"] = parameter_.beep.init;
    root["key"]["dev"] = parameter_.key.dev;

    root["serial"]["baud"] = parameter_.serial.baud;
    root["serial"]["dataBits"] = parameter_.serial.dataBits;
    root["serial"]["StopBits"] = parameter_.serial.stopBits;
    root["serial"]["parity"] = parameter_.serial.parity;
    root["serial"]["dev"] = parameter_.serial.dev;

    root["socket"]["ipaddr"] = parameter_.tcp.ipaddr;
    root["tcp"]["port"] = parameter_.tcp.port;
    root["logger"]["port"] = parameter_.logger.port;

    root["node"]["socket_port"] = parameter_.node.port;
    root["node"]["web_port"] = parameter_.web_node.port;
    root["rtc"]["dev"] = parameter_.rtc.dev;
    root["icmSpi"]["dev"] = parameter_.icmSpi.dev;
    root["apI2c"]["dev"] = parameter_.apI2c.dev;
    root["downloadpath"] = parameter_.downloadpath;

    Json::StreamWriterBuilder swb;
    std::shared_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    std::stringstream str;
    sw->write(root, &str);
    auto s = str.str();
    
    std::ofstream fstr;
    fstr.open(file_path_.c_str(), std::ios::out);
    fstr.write(s.c_str(), s.size());
    fstr.close();
}

std::ostream& operator<<(std::ostream& os, const system_config& config)
{
    const SystemParamter* parameter_ = &(config.parameter_);

    os<<"filepath:"<<config.getFilePath()<<"\n";
    os<<"led:"<<parameter_->led.dev<<" "<<parameter_->led.init<<"\n";
    os<<"beep:"<<parameter_->beep.dev<<" "<<parameter_->beep.init<<"\n";
    
    os<<"serial:"<<parameter_->serial.dev<<" "<<parameter_->serial.baud<<" "
    <<parameter_->serial.dataBits<<parameter_->serial.stopBits<<" "
    <<parameter_->serial.parity<<"\n";

    os<<"tcp:"<<parameter_->tcp.ipaddr<<" "<<parameter_->tcp.port<<"\n";
    os<<"logger:"<<parameter_->logger.ipaddr<<" "<<parameter_->logger.port<<"\n";
    os<<"node:"<<parameter_->node.ipaddr<<" "<<parameter_->node.port<<"\n";
    os<<"rtc:"<<parameter_->rtc.dev<<"\n";
    os<<"icmSpi:"<<parameter_->icmSpi.dev<<"\n";
    os<<"apI2c:"<<parameter_->apI2c.dev<<"\n";
    os<<"downloadpath:"<<parameter_->downloadpath;
    return os;
}

const uint8_t *get_version()
{
    return fw_version;
}