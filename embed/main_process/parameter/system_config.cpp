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
        parameter_.local_ipaddress              = root["local_ipaddress"].asString();
        parameter_.ipaddress                    = root["ipaddress"].asString();
        parameter_.version                      = root["version"].asString();
        
        parameter_.local_device.led.init        = root["local_device"]["led"]["init"].asInt();
        parameter_.local_device.led.dev         = root["local_device"]["led"]["dev"].asString();
        parameter_.local_device.beep.init       = root["local_device"]["beep"]["init"].asInt();
        parameter_.local_device.beep.dev        = root["local_device"]["beep"]["dev"].asString();
        parameter_.local_device.key.dev         = root["local_device"]["key"]["dev"].asString();
        parameter_.local_device.icm_spi.dev     = root["local_device"]["icm_spi"]["dev"].asString();
        parameter_.local_device.ap_i2c.dev      = root["local_device"]["ap_i2c"]["dev"].asString();
        parameter_.local_device.rtc.dev         = root["local_device"]["rtc"]["dev"].asString();

        parameter_.lower_device.serial.baud     = root["lower_device"]["serial"]["baud"].asInt();
        parameter_.lower_device.serial.dataBits = root["lower_device"]["serial"]["dataBits"].asInt();
        parameter_.lower_device.serial.stopBits = root["lower_device"]["serial"]["stopBits"].asInt();
        parameter_.lower_device.serial.parity   = root["lower_device"]["serial"]["parity"].asString();
        parameter_.lower_device.serial.dev      = root["lower_device"]["serial"]["dev"].asString();
    
        parameter_.main_process.download_path       = root["main_process"]["download_path"].asString();
        parameter_.main_process.node_port           = root["main_process"]["node_port"].asInt();
        parameter_.main_process.lower_deivce_port   = root["main_process"]["lower_deivce_port"].asInt();
        parameter_.main_process.local_device_port   = root["main_process"]["local_device_port"].asInt();
        parameter_.main_process.logger_port         = root["main_process"]["logger_port"].asInt();
        parameter_.main_process.gui_device_port     = root["main_process"]["gui_device_port"].asInt();
        
        parameter_.node_sever.web_port          = root["node_sever"]["web_port"].asInt();

        //save_config_file();
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

    parameter_.lower_device.serial.baud     = DEFAULT_SERIAL_BAUD;
    parameter_.lower_device.serial.dataBits = DEFAULT_SERIAL_DATABITS;
    parameter_.lower_device.serial.stopBits = DEFAULT_SERIAL_STOPBITS;
    parameter_.lower_device.serial.parity   = DEFAULT_SERIAL_PARITY;
    parameter_.lower_device.serial.dev      = DEFAULT_SERIAL_DEV;

    parameter_.main_process.download_path       = DEFAULT_DOWNLOAD_PATH;
    parameter_.main_process.node_port           = DEFAULT_NODE_PORT;
    parameter_.main_process.lower_deivce_port   = DEFAULT_LOWER_DEVICE_PORT;
    parameter_.main_process.local_device_port   = DEFAULT_LOCAL_DEVICE_PORT;
    parameter_.main_process.logger_port         = DEFAULT_LOGGER_PORT;
    parameter_.main_process.gui_device_port     = DEFAULT_GUI_DEVICE_PORT;
    
    parameter_.node_sever.web_port          = DEFAULT_NODE_WEB_PORT;
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

    root["lower_device"]["serial"]["baud"]      = parameter_.lower_device.serial.baud ;
    root["lower_device"]["serial"]["dataBits"]  = parameter_.lower_device.serial.dataBits;
    root["lower_device"]["serial"]["stopBits"]  = parameter_.lower_device.serial.stopBits;
    root["lower_device"]["serial"]["parity"]    = parameter_.lower_device.serial.parity;
    root["lower_device"]["serial"]["dev"]       = parameter_.lower_device.serial.dev;

    root["main_process"]["download_path"]       = parameter_.main_process.download_path;
    root["main_process"]["node_port"]           = parameter_.main_process.node_port;
    root["main_process"]["lower_deivce_port"]   = parameter_.main_process.lower_deivce_port;
    root["main_process"]["local_device_port"]   = parameter_.main_process.local_device_port;
    root["main_process"]["logger_port"]         = parameter_.main_process.logger_port;
    root["main_process"]["gui_device_port"]     = parameter_.main_process.gui_device_port;

    root["node_sever"]["web_port"]              = parameter_.node_sever.web_port;

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

    os<<"filepath:"<<config.get_config_file_path()<<"\n";
    os<<"local_ipaddress:"<<sys_parameter_->local_ipaddress<<"\n";
    os<<"ipaddress:"<<sys_parameter_->ipaddress<<"\n";
    os<<"version:"<<sys_parameter_->version<<"\n";

    os<<"led:"<<sys_parameter_->local_device.led.dev<<" "<<sys_parameter_->local_device.led.init<<"\n";
    os<<"beep:"<<sys_parameter_->local_device.beep.dev<<" "<<sys_parameter_->local_device.beep.init<<"\n";
    os<<"key:"<<sys_parameter_->local_device.key.dev<<"\n";
    os<<"icm_spi:"<<sys_parameter_->local_device.icm_spi.dev<<"\n";
    os<<"ap_i2c:"<<sys_parameter_->local_device.ap_i2c.dev<<"\n";
    os<<"rtc:"<<sys_parameter_->local_device.rtc.dev<<"\n";

    os<<"baud:"<<sys_parameter_->lower_device.serial.baud<<"\n";
    os<<"dataBits:"<<sys_parameter_->lower_device.serial.dataBits<<"\n";
    os<<"stopBits:"<<sys_parameter_->lower_device.serial.stopBits<<"\n";
    os<<"parity:"<<sys_parameter_->lower_device.serial.parity<<"\n";
    os<<"serial dev:"<<sys_parameter_->lower_device.serial.dev<<"\n";

    os<<"download:"<<sys_parameter_->main_process.download_path<<"\n";
    os<<"node port:"<<sys_parameter_->main_process.node_port<<"\n";
    os<<"lower device port:"<<sys_parameter_->main_process.lower_deivce_port<<"\n";
    os<<"local device port:"<<sys_parameter_->main_process.local_device_port<<"\n";
    os<<"logger port:"<<sys_parameter_->main_process.logger_port<<"\n";
    os<<"gui device port:"<<sys_parameter_->main_process.gui_device_port<<"\n";

    os<<"node port:"<<sys_parameter_->main_process.node_port<<"\n";

    return os;
}
