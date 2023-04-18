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
        parameter_.led.init         = root["device"]["led"]["init"].asInt();
        parameter_.led.dev          = root["device"]["led"]["path"].asString();
        parameter_.beep.init        = root["device"]["beep"]["init"].asInt();
        parameter_.beep.dev         = root["device"]["beep"]["path"].asString();
        parameter_.key.dev          = root["device"]["key"]["path"].asString();
        parameter_.serial.baud      = root["device"]["serial"]["baud"].asInt();
        parameter_.serial.dataBits  = root["device"]["serial"]["dataBits"].asInt();;
        parameter_.serial.stopBits  = root["device"]["serial"]["stopBits"].asInt();;
        parameter_.serial.parity    = root["device"]["serial"]["parity"].asString();;
        parameter_.serial.dev       = root["device"]["serial"]["path"].asString();
        parameter_.rtc.dev          = root["device"]["rtc"]["path"].asString();
        parameter_.icmSpi.dev       = root["device"]["icmSpi"]["path"].asString();; 
        parameter_.apI2c.dev        = root["device"]["apI2c"]["path"].asString();;

        parameter_.tcp.ipaddr       = root["socket"]["ipaddr"].asString();
        parameter_.tcp.port         = root["socket"]["tcp_port"].asInt();
        parameter_.logger.ipaddr    = parameter_.tcp.ipaddr;
        parameter_.logger.port      = root["socket"]["logger_port"].asInt();
        parameter_.node.ipaddr      = LOCAL_HOST;
        parameter_.node.port        = root["socket"]["internal_port"].asInt();
        parameter_.web_node.port    = root["node"]["web_port"].asInt();

        parameter_.downloadpath     = root["filepath"].asString();

        //SaveConfigFile();
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

bool system_config::update_fw_info()
{
    Json::Value root;
    std::ifstream ifs;

    parameter_.information.author = DEFAULT_FW_AUTHOR;
    parameter_.information.version = DEFAULT_FW_VERSION;

    ifs.open(DEFAULT_FW_INFO_FILE);
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

    try
    {
        parameter_.information.author = root["package"]["author"].asString();
        parameter_.information.version = root["package"]["version"].asString();
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
}

void system_config::SaveConfigFile()
{
    Json::Value root;

    //gpio
    root["device"]["led"]["init"]           = parameter_.led.init;
    root["device"]["led"]["path"]           = parameter_.led.dev;
    root["device"]["beep"]["init"]          = parameter_.beep.init;
    root["device"]["beep"]["path"]          = parameter_.beep.dev;
    root["device"]["key"]["path"]           = parameter_.key.dev;
    root["device"]["serial"]["baud"]        = parameter_.serial.baud;
    root["device"]["serial"]["dataBits"]    = parameter_.serial.dataBits;
    root["device"]["serial"]["StopBits"]    = parameter_.serial.stopBits;
    root["device"]["serial"]["parity"]      = parameter_.serial.parity;
    root["device"]["serial"]["path"]        = parameter_.serial.dev;
    root["device"]["rtc"]["path"]           = parameter_.rtc.dev;
    root["device"]["icmSpi"]["path"]        = parameter_.icmSpi.dev;
    root["device"]["apI2c"]["path"]         = parameter_.apI2c.dev;

    root["socket"]["ipaddr"]                = parameter_.tcp.ipaddr;
    root["socket"]["tcp_port"]              = parameter_.tcp.port;
    root["socket"]["logger_port"]           = parameter_.logger.port;
    root["socket"]["internal_port"]         = parameter_.node.port;
    root["node"]["web_port"]                = parameter_.web_node.port;
    
    root["filepath"]                        = parameter_.downloadpath;

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
