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
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>  
#include <fstream>
#include <iostream>
#include <memory>

#include "common.hpp"
#include "jsonconfig.hpp"
#include "logger_manage.hpp"

system_config* system_config::instance_pointer_ = nullptr;
system_config* system_config::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) system_config();
        if (instance_pointer_ == nullptr)
        {
            //to do error process
        }
    }
    return instance_pointer_;
}

bool system_config::check_configfile(const std::string &ipaddr)
{
    struct ifaddrs *ifaddr, *ifa;
    char host[NI_MAXHOST];  
    bool is_check = false;

    if (getifaddrs(&ifaddr) == -1) {  
        perror("getifaddrs");  
        exit(EXIT_FAILURE);
        return false;
    }  
  
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {  
        if (ifa->ifa_addr == NULL) 
        {  
            continue;  
        }  
        int family = ifa->ifa_addr->sa_family;  
        if (family == AF_INET) 
        { // IPv4 or IPv6  
            if (getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST) == 0) 
            {  
                PRINT_NOW("%s:%s IpAddress:%s, %s\n", PRINT_NOW_HEAD_STR, ifa->ifa_name, host, ipaddr.c_str());
                if (ipaddr == std::string(host))
                {
                    is_check = true;
                    break;
                }
            }  
        }  
    }  
    freeifaddrs(ifaddr);

    if(parameter_.main_process.mqtt_device.id.empty())
    {
        is_check = false;
    }
    return is_check;
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
    if (!ifs.is_open())
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
        parameter_.local_ipaddress                  = root["local_ipaddress"].asString();
        parameter_.ipaddress                        = root["ipaddress"].asString();
        parameter_.version                          = root["version"].asString();
        
        parameter_.local_device.led.init            = root["local_device"]["led"]["init"].asInt();
        parameter_.local_device.led.dev             = root["local_device"]["led"]["dev"].asString();
        parameter_.local_device.beep.init           = root["local_device"]["beep"]["init"].asInt();
        parameter_.local_device.beep.dev            = root["local_device"]["beep"]["dev"].asString();
        parameter_.local_device.key.dev             = root["local_device"]["key"]["dev"].asString();
        parameter_.local_device.icm_spi.dev         = root["local_device"]["icm_spi"]["dev"].asString();
        parameter_.local_device.ap_i2c.dev          = root["local_device"]["ap_i2c"]["dev"].asString();
        parameter_.local_device.rtc.dev             = root["local_device"]["rtc"]["dev"].asString();

        //main process
        parameter_.main_process.download_path           = root["main_process"]["download_path"].asString();
        parameter_.main_process.node_port               = root["main_process"]["node_port"].asInt();
        parameter_.main_process.local_port              = root["main_process"]["local_port"].asInt();
        parameter_.main_process.logger_port             = root["main_process"]["logger_port"].asInt();
        parameter_.main_process.gui_port                = root["main_process"]["gui_port"].asInt();
        parameter_.main_process.mqtt_device.id            = root["main_process"]["mqtt_device"]["id"].asString();
        parameter_.main_process.mqtt_device.port          = root["main_process"]["mqtt_device"]["port"].asInt();
        parameter_.main_process.mqtt_device.sub_topic     = root["main_process"]["mqtt_device"]["sub_topic"].asString();
        parameter_.main_process.mqtt_device.pub_topic     = root["main_process"]["mqtt_device"]["pub_topic"].asString();
        parameter_.main_process.mqtt_device.keepalive     = root["main_process"]["mqtt_device"]["keepalive"].asInt();
        parameter_.main_process.mqtt_device.qos           = root["main_process"]["mqtt_device"]["qos"].asInt();

        parameter_.node_sever.web_port              = root["node_sever"]["web_port"].asInt();

        parameter_.lower_device.logger_port         = root["lower_device"]["logger_port"].asInt();
        parameter_.lower_device.serial.baud         = root["lower_device"]["serial"]["baud"].asInt();
        parameter_.lower_device.serial.dataBits     = root["lower_device"]["serial"]["dataBits"].asInt();
        parameter_.lower_device.serial.stopBits     = root["lower_device"]["serial"]["stopBits"].asInt();
        parameter_.lower_device.serial.parity       = root["lower_device"]["serial"]["parity"].asString();
        parameter_.lower_device.serial.dev          = root["lower_device"]["serial"]["dev"].asString();
        parameter_.lower_device.serial.net_port     = root["lower_device"]["serial"]["net_port"].asInt();
        //save_config_file();
    }
    catch(const std::exception& e)
    {
        ifs.close();
        std::cerr << e.what() << '\n';
        return false;
    }
    
    ifs.close();

    return check_configfile(parameter_.ipaddress);
}

void system_config::default_init() noexcept
{
    parameter_.local_ipaddress              = DEFAULT_LOCAL_IPADRESS;
    parameter_.ipaddress                    = DEFAULT_IPADDRESS;
    parameter_.version                      = DEFAULT_FW_VERSION;
    
    parameter_.local_device.led.init        = DEFAULT_LED_INIT;
    parameter_.local_device.led.dev         = DEFAULT_LED_DEV;
    parameter_.local_device.beep.init       = DEFAULT_BEEP_INIT;
    parameter_.local_device.beep.dev        = DEFAULT_BEEP_DEV;
    parameter_.local_device.key.dev         = DEFAULT_KEY_DEV;
    parameter_.local_device.icm_spi.dev     = DEFAULT_ICMSPI_DEV;
    parameter_.local_device.ap_i2c.dev      = DEFAULT_API2C_DEV;
    parameter_.local_device.rtc.dev         = DEFAULT_RTC_DEV;

    parameter_.main_process.download_path   = DEFAULT_DOWNLOAD_PATH;
    parameter_.main_process.node_port       = DEFAULT_NODE_PORT;
    parameter_.main_process.local_port      = DEFAULT_LOCAL_DEVICE_PORT;
    parameter_.main_process.logger_port     = DEFAULT_LOGGER_PORT;
    parameter_.main_process.gui_port        = DEFAULT_GUI_DEVICE_PORT;
    parameter_.main_process.mqtt_device.id    = "default";
    parameter_.main_process.mqtt_device.port  = 1883;
    parameter_.main_process.mqtt_device.sub_topic = "/info/null0",
    parameter_.main_process.mqtt_device.pub_topic = "/info/null1",
    parameter_.main_process.mqtt_device.keepalive = 60;
    parameter_.main_process.mqtt_device.qos   = 0;

    parameter_.node_sever.web_port          = DEFAULT_NODE_WEB_PORT;

    parameter_.lower_device.serial.baud     = DEFAULT_SERIAL_BAUD;
    parameter_.lower_device.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter_.lower_device.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter_.lower_device.serial.parity   = DEFAULT_SERIAL_PARITY;
    parameter_.lower_device.serial.dev      = DEFAULT_SERIAL_DEV;
}

void system_config::save_config_file()
{
    Json::Value root;

    root["local_ipaddress"]                     = parameter_.local_ipaddress;
    root["ipaddress"]                           = parameter_.ipaddress;
    root["version"]                             = parameter_.version ;

    root["local_device"]["led"]["init"]         = parameter_.local_device.led.init;
    root["local_device"]["led"]["dev"]          = parameter_.local_device.led.dev;

    root["local_device"]["beep"]["init"]        = parameter_.local_device.beep.init;
    root["local_device"]["beep"]["dev"]         = parameter_.local_device.beep.dev;
    root["local_device"]["key"]["dev"]          = parameter_.local_device.key.dev;
    root["local_device"]["icm_spi"]["dev"]      = parameter_.local_device.icm_spi.dev;
    root["local_device"]["ap_i2c"]["dev"]       = parameter_.local_device.ap_i2c.dev;
    root["local_device"]["rtc"]["dev"]          = parameter_.local_device.rtc.dev  ;

    root["main_process"]["download_path"]       = parameter_.main_process.download_path;
    root["main_process"]["node_port"]           = parameter_.main_process.node_port;
    root["main_process"]["local_port"]          = parameter_.main_process.local_port;
    root["main_process"]["logger_port"]         = parameter_.main_process.logger_port;
    root["main_process"]["gui_port"]            = parameter_.main_process.gui_port;
    root["main_process"]["mqtt_device"]["id"]     = parameter_.main_process.mqtt_device.id; 
    root["main_process"]["mqtt_device"]["port"]   = parameter_.main_process.mqtt_device.port;
    root["main_process"]["mqtt_device"]["sub_topic"] = parameter_.main_process.mqtt_device.sub_topic;
    root["main_process"]["mqtt_device"]["pub_topic"] = parameter_.main_process.mqtt_device.pub_topic;
    root["main_process"]["mqtt_device"]["keepalive"] = parameter_.main_process.mqtt_device.keepalive;
    root["main_process"]["mqtt_device"]["qos"]       = parameter_.main_process.mqtt_device.qos;
        
    root["node_sever"]["web_port"]              = parameter_.node_sever.web_port;

    root["lower_device"]["logger_port"]         = parameter_.lower_device.logger_port;   
    root["lower_device"]["serial"]["baud"]      = parameter_.lower_device.serial.baud ;
    root["lower_device"]["serial"]["dataBits"]  = parameter_.lower_device.serial.dataBits;
    root["lower_device"]["serial"]["stopBits"]  = parameter_.lower_device.serial.stopBits;
    root["lower_device"]["serial"]["parity"]    = parameter_.lower_device.serial.parity;
    root["lower_device"]["serial"]["dev"]       = parameter_.lower_device.serial.dev;

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
    const SystemParamter* sys_parameter_ = &(config.parameter_);

    os<<"------------------main info ------------------------------\n";
    os<<"filepath:"<<config.get_config_file_path()<<"\n";
    os<<"local_ipaddress:"<<config.get_local_ipaddress()<<"\n";
    os<<"ipaddress:"<<config.get_ipaddress()<<"\n";
    os<<"version:"<<config.get_version()<<"\n";

    os<<"------------------local device info ------------------------------\n";
    os<<"led:"<<config.get_led_config().dev<<" "<<config.get_led_config().init<<"\n";
    os<<"beep:"<<config.get_beep_config().dev<<" "<<config.get_beep_config().init<<"\n";
    os<<"key:"<<config.get_key_config().dev<<"\n";
    os<<"icm_spi:"<<config.get_icm_config().dev<<"\n";
    os<<"ap_i2c:"<<config.get_ap_config().dev<<"\n";
    os<<"rtc:"<<config.get_rtc_config().dev<<"\n";

    os<<"------------------lower device info ------------------------------\n";
    os<<"logger port:"<<sys_parameter_->lower_device.logger_port<<"\n"; 
    os<<"net port:"<<sys_parameter_->lower_device.serial.net_port<<"\n"; 
    os<<"baud:"<<sys_parameter_->lower_device.serial.baud<<"\n";
    os<<"dataBits:"<<sys_parameter_->lower_device.serial.dataBits<<"\n";
    os<<"stopBits:"<<sys_parameter_->lower_device.serial.stopBits<<"\n";
    os<<"parity:"<<sys_parameter_->lower_device.serial.parity<<"\n";
    os<<"serial dev:"<<sys_parameter_->lower_device.serial.dev<<"\n";

    os<<"------------------main process info ------------------------------\n";
    os<<"download:"<<sys_parameter_->main_process.download_path<<"\n";
    os<<"node port:"<<sys_parameter_->main_process.node_port<<"\n";
    os<<"local port:"<<sys_parameter_->main_process.local_port<<"\n";
    os<<"logger port:"<<sys_parameter_->main_process.logger_port<<"\n";
    os<<"gui port:"<<sys_parameter_->main_process.gui_port<<"\n";
    os<<"mqtt id:"<<sys_parameter_->main_process.mqtt_device.id<<"\n";
    os<<"mqtt port:"<<sys_parameter_->main_process.mqtt_device.port<<"\n";
    os<<"mqtt keepalive:"<<sys_parameter_->main_process.mqtt_device.keepalive<<"\n";
    os<<"mqtt pub_topic:"<<sys_parameter_->main_process.mqtt_device.pub_topic<<"\n";
    os<<"mqtt sub_topic:"<<sys_parameter_->main_process.mqtt_device.sub_topic<<"\n";
    os<<"mqtt qos:"<<sys_parameter_->main_process.mqtt_device.qos<<"\n"; 

    os<<"------------------node server info ------------------------------\n";
    os<<"node port:"<<config.get_node_port()<<"\n";

    return os;
}
