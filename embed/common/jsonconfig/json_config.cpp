//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      json_config.cpp
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
#include "json_config.hpp"
#include "logger_manage.hpp"
#include "timer_manage.hpp"

#define DYNAMIC_CONFIG_PATH "/home/sys/configs/dynamic.json"

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
                PRINT_NOW("%s:%s IpAddress Diff %s, %s\n", PRINT_NOW_HEAD_STR, ifa->ifa_name, host, ipaddr.c_str());
                if (ipaddr == std::string(host))
                {
                    is_check = true;
                    break;
                }
            }
        }
    }
    freeifaddrs(ifaddr);

    if (parameter_.main_process.mqtt_device.id.empty())
    {
        is_check = false;
    }
    return is_check;
}

bool system_config::dynamic_init(const char* path)
{
    Json::Value root;
    std::ifstream ifs;
    
    dynamic_path_.clear();
    dynamic_path_.assign(path);
    
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

    try
    {
        parameter_.logger_privilege.gui_manage_level = root["logger_privilege"]["gui_manage_level"].asInt();
        parameter_.logger_privilege.local_device_level = root["logger_privilege"]["local_device_level"].asInt();
        parameter_.logger_privilege.logger_device_level = root["logger_privilege"]["logger_device_level"].asInt();
        parameter_.logger_privilege.lower_device_level = root["logger_privilege"]["lower_device_level"].asInt();
        parameter_.logger_privilege.main_process_level = root["logger_privilege"]["main_process_level"].asInt();
        parameter_.logger_privilege.node_server_level = root["logger_privilege"]["node_server_level"].asInt();

        parameter_.logger_privilege.allow_no_passwd = root["logger_manage"]["allow_no_passwd"].asBool();
        parameter_.logger_privilege.passwd = root["logger_manage"]["passwd"].asString();
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
        parameter_.mqtt_host                        = root["mqtt_host"].asString();
        parameter_.mqtt_port                        = root["mqtt_port"].asInt();
        parameter_.version                          = root["version"].asString();
        
        parameter_.local_device.led.init            = root["local_device"]["led"]["init"].asInt();
        parameter_.local_device.led.dev             = root["local_device"]["led"]["dev"].asString();
        parameter_.local_device.beep.init           = root["local_device"]["beep"]["init"].asInt();
        parameter_.local_device.beep.dev            = root["local_device"]["beep"]["dev"].asString();
        parameter_.local_device.loopled.dev         = root["local_device"]["loopled"]["dev"].asString();
        parameter_.local_device.key.dev             = root["local_device"]["key"]["dev"].asString();
        parameter_.local_device.icm_spi.dev         = root["local_device"]["icm_spi"]["dev"].asString();
        parameter_.local_device.ap_i2c.dev          = root["local_device"]["ap_i2c"]["dev"].asString();
        parameter_.local_device.rtc.dev             = root["local_device"]["rtc"]["dev"].asString();
        parameter_.local_device.iio.vf610_adc_dev   = root["local_device"]["iio"]["vf610_adc"].asString();
        parameter_.local_device.iio.hx711_dev       = root["local_device"]["iio"]["hx711_dev"].asString();
        parameter_.local_device.pwm.pwm_chip        = root["local_device"]["pwm"]["dev"].asString();
        parameter_.local_device.pwm.state           = root["local_device"]["pwm"]["init"]["state"].asInt();
        parameter_.local_device.pwm.peroid          = root["local_device"]["pwm"]["init"]["peroid"].asUInt();
        parameter_.local_device.pwm.duty_cycle       = root["local_device"]["pwm"]["init"]["duty"].asUInt();

        //main process
        parameter_.main_process.download_path           = root["main_process"]["download_path"].asString();
        parameter_.main_process.node_port               = root["main_process"]["node_port"].asInt();
        parameter_.main_process.local_port              = root["main_process"]["local_port"].asInt();
        parameter_.main_process.logger_port             = root["main_process"]["logger_port"].asInt();
        parameter_.main_process.gui_port                = root["main_process"]["gui_port"].asInt();
        parameter_.main_process.mqtt_device.id            = root["main_process"]["mqtt_device"]["id"].asString();
        parameter_.main_process.mqtt_device.sub_topic     = root["main_process"]["mqtt_device"]["sub_topic"].asString();
        parameter_.main_process.mqtt_device.keepalive     = root["main_process"]["mqtt_device"]["keepalive"].asInt();
        parameter_.main_process.mqtt_device.qos           = root["main_process"]["mqtt_device"]["qos"].asInt();
        parameter_.main_process.serial.stopBits           = root["main_process"]["serial"]["stopBits"].asInt();
        parameter_.main_process.serial.baud           = root["main_process"]["serial"]["baud"].asInt();
        parameter_.main_process.serial.dataBits       = root["main_process"]["serial"]["dataBits"].asInt();
        parameter_.main_process.serial.parity         = root["main_process"]["serial"]["parity"].asString();
        parameter_.main_process.serial.dev            = root["main_process"]["serial"]["dev"].asString();

        //node server
        parameter_.node_server.web_port              = root["node_server"]["web_port"].asInt();
        parameter_.node_server.pages                 = root["node_server"]["pages"].asString();
        parameter_.node_server.mqtt_device.id            = root["node_server"]["mqtt_device"]["id"].asString();
        parameter_.node_server.mqtt_device.sub_topic     = root["node_server"]["mqtt_device"]["sub_topic"].asString();
        parameter_.node_server.mqtt_device.keepalive     = root["node_server"]["mqtt_device"]["keepalive"].asInt();
        parameter_.node_server.mqtt_device.qos           = root["node_server"]["mqtt_device"]["qos"].asInt();

        parameter_.gui_manage.mqtt_device.id            = root["gui_manage"]["mqtt_device"]["id"].asString();
        parameter_.gui_manage.mqtt_device.sub_topic     = root["gui_manage"]["mqtt_device"]["sub_topic"].asString();
        parameter_.gui_manage.mqtt_device.keepalive     = root["gui_manage"]["mqtt_device"]["keepalive"].asInt();
        parameter_.gui_manage.mqtt_device.qos           = root["gui_manage"]["mqtt_device"]["qos"].asInt();

        parameter_.winform.mqtt_device.id            = root["winform"]["mqtt_device"]["id"].asString();
        parameter_.winform.mqtt_device.sub_topic     = root["winform"]["mqtt_device"]["sub_topic"].asString();
        parameter_.winform.mqtt_device.keepalive     = root["winform"]["mqtt_device"]["keepalive"].asInt();
        parameter_.winform.mqtt_device.qos           = root["winform"]["mqtt_device"]["qos"].asInt();

        parameter_.lower_device.logger_port         = root["lower_device"]["logger_port"].asInt();
        parameter_.lower_device.remote_port         = root["lower_device"]["remote_port"].asInt();
        parameter_.lower_device.serial.baud         = root["lower_device"]["serial"]["baud"].asInt();
        parameter_.lower_device.serial.dataBits     = root["lower_device"]["serial"]["dataBits"].asInt();
        parameter_.lower_device.serial.stopBits     = root["lower_device"]["serial"]["stopBits"].asInt();
        parameter_.lower_device.serial.parity       = root["lower_device"]["serial"]["parity"].asString();
        parameter_.lower_device.serial.dev          = root["lower_device"]["serial"]["dev"].asString();
    }
    catch(const std::exception& e)
    {
        ifs.close();
        std::cerr << e.what() << '\n';
        return false;
    }
    
    ifs.close();

    if (dynamic_init(DYNAMIC_CONFIG_PATH) != true)
    {
        return false;
    }
    
    return check_configfile(parameter_.ipaddress);
}

void system_config::default_init() noexcept
{
    parameter_.local_ipaddress              = DEFAULT_LOCAL_IPADRESS;
    parameter_.ipaddress                    = DEFAULT_IPADDRESS;
    parameter_.mqtt_host                    = DEFAULT_IPADDRESS;
    parameter_.mqtt_port                    = DEFAULT_MQTT_PORT;
    parameter_.version                      = DEFAULT_FW_VERSION;
    
    parameter_.local_device.led.init        = DEFAULT_LED_INIT;
    parameter_.local_device.led.dev         = DEFAULT_LED_DEV;
    parameter_.local_device.beep.init       = DEFAULT_BEEP_INIT;
    parameter_.local_device.beep.dev        = DEFAULT_BEEP_DEV;
    parameter_.local_device.loopled.dev     = DEFAULT_LOOPLED_DEV;
    parameter_.local_device.key.dev         = DEFAULT_KEY_DEV;
    parameter_.local_device.icm_spi.dev     = DEFAULT_ICMSPI_DEV;
    parameter_.local_device.ap_i2c.dev      = DEFAULT_API2C_DEV;
    parameter_.local_device.rtc.dev         = DEFAULT_RTC_DEV;
    parameter_.local_device.iio.hx711_dev     = DEFAULT_HX711_DEV;
    parameter_.local_device.iio.vf610_adc_dev = DEFAULT_VF610_DEV;
    parameter_.local_device.pwm.pwm_chip        = DEFAULT_PWM_CHIP;
    parameter_.local_device.pwm.state           = DEFAULT_PWM_STATE;
    parameter_.local_device.pwm.peroid          = DEFAULT_PWM_PEROID;
    parameter_.local_device.pwm.duty_cycle       = DEFAULT_PWM_DUTY;

    parameter_.main_process.download_path   = DEFAULT_DOWNLOAD_PATH;
    parameter_.main_process.node_port       = DEFAULT_NODE_PORT;
    parameter_.main_process.local_port      = DEFAULT_LOCAL_DEVICE_PORT;
    parameter_.main_process.logger_port     = DEFAULT_LOGGER_PORT;
    parameter_.main_process.gui_port        = DEFAULT_GUI_DEVICE_PORT;
    parameter_.main_process.mqtt_device.id    = "default";
    parameter_.main_process.mqtt_device.sub_topic = "/info/null0",
    parameter_.main_process.mqtt_device.keepalive = 60;
    parameter_.main_process.mqtt_device.qos   = 0;
    parameter_.main_process.serial.baud     = DEFAULT_SERIAL_BAUD;
    parameter_.main_process.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter_.main_process.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter_.main_process.serial.parity   = DEFAULT_SERIAL_PARITY;
    parameter_.main_process.serial.dev      = DEFAULT_SERIAL_DEV;
    
    parameter_.node_server.web_port          = DEFAULT_NODE_WEB_PORT;
    parameter_.node_server.pages             = DEFAULT_NODE_PAGES;
    parameter_.node_server.mqtt_device.id    = "default";
    parameter_.node_server.mqtt_device.sub_topic = "/info/null0";
    parameter_.node_server.mqtt_device.keepalive = 60;
    parameter_.node_server.mqtt_device.qos   = 0;

    parameter_.gui_manage.mqtt_device.id      = "default";
    parameter_.gui_manage.mqtt_device.sub_topic = "/info/null1";
    parameter_.gui_manage.mqtt_device.keepalive = 60;
    parameter_.gui_manage.mqtt_device.qos = 0;

    parameter_.winform.mqtt_device.id      = "default";
    parameter_.winform.mqtt_device.sub_topic = "/info/null2";
    parameter_.winform.mqtt_device.keepalive = 60;
    parameter_.winform.mqtt_device.qos = 0;

    parameter_.lower_device.logger_port = DEFAULT_LOWER_DEVICE_LOGGER_PORT;
    parameter_.lower_device.remote_port = DEFAULT_LOWER_DEVICE_REMOTE_PORT;
    parameter_.lower_device.serial.baud     = DEFAULT_SERIAL_BAUD;
    parameter_.lower_device.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter_.lower_device.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter_.lower_device.serial.parity   = DEFAULT_SERIAL_PARITY;
    parameter_.lower_device.serial.dev      = DEFAULT_SERIAL_DEV;

    parameter_.logger_privilege.gui_manage_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.local_device_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.logger_device_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.lower_device_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.main_process_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.node_server_level = DEFAULT_LOGGER_LEVEL;
    parameter_.logger_privilege.passwd = DEFAULT_LOGGER_PASSWD;
    parameter_.logger_privilege.allow_no_passwd = DEFAULT_ALLOW_NO_PASSWD;
}

void system_config::save_dynamicfile()
{
    Json::Value root;

    root["logger_privilege"]["gui_manage_level"] = parameter_.logger_privilege.gui_manage_level;
    root["logger_privilege"]["local_device_level"] = parameter_.logger_privilege.local_device_level;
    root["logger_privilege"]["logger_device_level"] = parameter_.logger_privilege.logger_device_level;
    root["logger_privilege"]["lower_device_level"] = parameter_.logger_privilege.lower_device_level;
    root["logger_privilege"]["main_process_level"] = parameter_.logger_privilege.main_process_level;
    root["logger_privilege"]["node_server_level"] = parameter_.logger_privilege.node_server_level;
    root["logger_manage"]["passwd"] = parameter_.logger_privilege.passwd;
    root["logger_manage"]["allow_no_passwd"] = parameter_.logger_privilege.allow_no_passwd;

    Json::StreamWriterBuilder swb;
    std::shared_ptr<Json::StreamWriter> sw(swb.newStreamWriter());

    std::stringstream str;
    sw->write(root, &str);
    auto s = str.str();
    
    std::ofstream fstr;
    fstr.open(dynamic_path_.c_str(), std::ios::out);
    fstr.write(s.c_str(), s.size());
    fstr.close();   
}

void system_config::save_configfile()
{
    Json::Value root;

    root["local_ipaddress"]                     = parameter_.local_ipaddress;
    root["ipaddress"]                           = parameter_.ipaddress;
    root["mqtt_host"]                           = parameter_.mqtt_host;
    root["mqtt_port"]                           = parameter_.mqtt_port;
    root["version"]                             = parameter_.version ;

    root["local_device"]["led"]["init"]         = parameter_.local_device.led.init;
    root["local_device"]["led"]["dev"]          = parameter_.local_device.led.dev;

    root["local_device"]["beep"]["init"]        = parameter_.local_device.beep.init;
    root["local_device"]["beep"]["dev"]         = parameter_.local_device.beep.dev;
    root["local_device"]["key"]["dev"]          = parameter_.local_device.key.dev;
    root["local_device"]["icm_spi"]["dev"]      = parameter_.local_device.icm_spi.dev;
    root["local_device"]["ap_i2c"]["dev"]       = parameter_.local_device.ap_i2c.dev;
    root["local_device"]["rtc"]["dev"]          = parameter_.local_device.rtc.dev  ;
    root["local_device"]["loopled"]["dev"]       = parameter_.local_device.loopled.dev  ;

    root["local_device"]["pwm"]["dev"] = parameter_.local_device.pwm.pwm_chip;
    root["local_device"]["pwm"]["init"]["state"] = parameter_.local_device.pwm.state;
    root["local_device"]["pwm"]["init"]["peroid"] = parameter_.local_device.pwm.peroid;
    root["local_device"]["pwm"]["init"]["duty"] =  parameter_.local_device.pwm.duty_cycle;

    root["main_process"]["download_path"]       = parameter_.main_process.download_path;
    root["main_process"]["node_port"]           = parameter_.main_process.node_port;
    root["main_process"]["local_port"]          = parameter_.main_process.local_port;
    root["main_process"]["logger_port"]         = parameter_.main_process.logger_port;
    root["main_process"]["gui_port"]            = parameter_.main_process.gui_port;
    root["main_process"]["mqtt_device"]["id"]     = parameter_.main_process.mqtt_device.id; 
    root["main_process"]["mqtt_device"]["sub_topic"] = parameter_.main_process.mqtt_device.sub_topic;
    root["main_process"]["mqtt_device"]["keepalive"] = parameter_.main_process.mqtt_device.keepalive;
    root["main_process"]["mqtt_device"]["qos"]       = parameter_.main_process.mqtt_device.qos;
    root["main_process"]["serial"]["stopBits"]       = parameter_.main_process.serial.stopBits;
    root["main_process"]["serial"]["baud"]      =  parameter_.main_process.serial.baud ;
    root["main_process"]["serial"]["dataBits"]  =  parameter_.main_process.serial.dataBits;
    root["main_process"]["serial"]["parity"]    =  parameter_.main_process.serial.parity;
    root["main_process"]["serial"]["dev"] =    parameter_.main_process.serial.dev;
     
    root["node_server"]["web_port"]              = parameter_.node_server.web_port;
    root["node_server"]["pages"]                 = parameter_.node_server.pages;
    root["node_server"]["mqtt_device"]["id"]     = parameter_.node_server.mqtt_device.id; 
    root["node_server"]["mqtt_device"]["sub_topic"] = parameter_.node_server.mqtt_device.sub_topic;
    root["node_server"]["mqtt_device"]["keepalive"] = parameter_.node_server.mqtt_device.keepalive;
    root["node_server"]["mqtt_device"]["qos"]       = parameter_.node_server.mqtt_device.qos;

    root["gui_manage"]["mqtt_device"]["id"]     = parameter_.gui_manage.mqtt_device.id; 
    root["gui_manage"]["mqtt_device"]["sub_topic"] = parameter_.gui_manage.mqtt_device.sub_topic;
    root["gui_manage"]["mqtt_device"]["keepalive"] = parameter_.gui_manage.mqtt_device.keepalive;
    root["gui_manage"]["mqtt_device"]["qos"]       = parameter_.gui_manage.mqtt_device.qos;

    root["winform"]["mqtt_device"]["id"]     = parameter_.winform.mqtt_device.id;
    root["winform"]["mqtt_device"]["sub_topic"] = parameter_.winform.mqtt_device.sub_topic;
    root["winform"]["mqtt_device"]["keepalive"] = parameter_.winform.mqtt_device.keepalive;
    root["winform"]["mqtt_device"]["qos"]       = parameter_.winform.mqtt_device.qos;

    root["lower_device"]["logger_port"]         = parameter_.lower_device.logger_port;
    root["lower_device"]["remote_port"]         = parameter_.lower_device.remote_port; 
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

bool system_config::set_logger_level(int dev, int level)
{
    int ret = -1;

    switch (dev)
    {
        case GUI_LOGGER_DEV:
            ret = 0;
            if (parameter_.logger_privilege.gui_manage_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.gui_manage_level = level;
            }
            break;
        case LOCAL_LOGGER_DEV:
            ret = 0;
            if (parameter_.logger_privilege.local_device_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.local_device_level = level;
            }
            break;
        case LOGGER_LOGGER_DEV:
            ret = 0;
            if (parameter_.logger_privilege.logger_device_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.logger_device_level = level;
            }
            break;
        case LOWER_LOGGER_DEV:
            ret = 0;
            if (parameter_.logger_privilege.lower_device_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.lower_device_level = level;
            }
            break;
        case MAIN_LOGGER_DEV:
            ret = 0;
            if (parameter_.logger_privilege.main_process_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.main_process_level = level;
            }
            break;
        case NODE_LOGGGE_DEV:
            ret = 0;
            if (parameter_.logger_privilege.node_server_level != level)
            {
                ret = 1;
                parameter_.logger_privilege.node_server_level = level;
            }
            break;
    }

    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "set level:%d, %d, ret:%d", dev, level, ret);
    if (ret == 1)
    {
        save_dynamicfile();
    }
    return ret;
}

std::ostream& operator<<(std::ostream& os, const system_config& config)
{
    const SystemParamter* sys_parameter_ = &(config.parameter_);

    os<<"------------------main info ------------------------------\n";
    os<<"filepath:"<<config.get_config_file_path()<<"\n";
    os<<"local_ipaddress:"<<config.get_local_ipaddress()<<"\n";
    os<<"ipaddress:"<<config.get_ipaddress()<<"\n";
    os<<"mqtt_host:"<<config.get_mqtthost()<<"\n";
    os<<"mqtt_port:"<<config.get_mqttport()<<"\n";
    os<<"version:"<<config.get_version()<<"\n";

    os<<"------------------local device info ------------------------------\n";
    os<<"led:"<<config.get_led_config().dev<<" "<<config.get_led_config().init<<"\n";
    os<<"beep:"<<config.get_beep_config().dev<<" "<<config.get_beep_config().init<<"\n";
    os<<"loopled"<<config.get_loopled_config().dev<<"\n";
    os<<"key:"<<config.get_key_config().dev<<"\n";
    os<<"icm_spi:"<<config.get_icm_config().dev<<"\n";
    os<<"ap_i2c:"<<config.get_ap_config().dev<<"\n";
    os<<"rtc:"<<config.get_rtc_config().dev<<"\n";
    os<<"hx711:"<<config.get_iio_config().hx711_dev<<"\n";
    os<<"vf610:"<<config.get_iio_config().vf610_adc_dev<<"\n";

    os<<"------------------lower device info ------------------------------\n";
    os<<"logger port:"<<sys_parameter_->lower_device.logger_port<<"\n"; 
    os<<"net port:"<<sys_parameter_->lower_device.remote_port<<"\n"; 
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
    os<<"mqtt keepalive:"<<sys_parameter_->main_process.mqtt_device.keepalive<<"\n";
    os<<"mqtt sub_topic:"<<sys_parameter_->main_process.mqtt_device.sub_topic<<"\n";
    os<<"mqtt qos:"<<sys_parameter_->main_process.mqtt_device.qos<<"\n"; 
    os<<"serial baud:"<<sys_parameter_->main_process.serial.baud<<"\n";
    os<<"serial dataBits:"<<sys_parameter_->main_process.serial.dataBits<<"\n";
    os<<"serial stopBits:"<<sys_parameter_->main_process.serial.stopBits<<"\n";
    os<<"serial parity:"<<sys_parameter_->main_process.serial.parity<<"\n";
    os<<"serial serial dev:"<<sys_parameter_->main_process.serial.dev<<"\n";

    os<<"------------------node server info ------------------------------\n";
    os<<"node port:"<<config.get_node_port()<<"\n";

    os<<"------------------ privilege ------------------------------\n";
    os<<"gui_manage_level:"<<config.get_logger_privilege().gui_manage_level<<"\n";
    os<<"local_dev_level:"<<config.get_logger_privilege().local_device_level<<"\n";
    os<<"logger_dev_level:"<<config.get_logger_privilege().logger_device_level<<"\n";
    os<<"lower_dev_level:"<<config.get_logger_privilege().lower_device_level<<"\n";
    os<<"main_process_level:"<<config.get_logger_privilege().main_process_level<<"\n";
    os<<"node_server_level:"<<config.get_logger_privilege().node_server_level<<"\n";
    return os;
}
