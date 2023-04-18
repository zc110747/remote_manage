//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      system_config.hpp
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
    JString author;
    JString version;
}fw_information;

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
    fw_information information;
}SystemParamter;

class system_config
{
    friend std::ostream& operator<<(std::ostream&, const system_config&);

public:
    /// \brief constructor
    system_config() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~system_config() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static system_config* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \param path - path of the file for config.
    /// \return Wheather initialization is success or failed.
    bool init(const char* path);

    /// \brief update_fw_info
    /// - This method is used to update fw information.
    bool update_fw_info();

    /// \brief default_init
    /// - This method is used to set all data default.
    void default_init() noexcept;

    /// \brief save_config_file
    /// - This method is used to save config file.
    void save_config_file();

public:
    /// \brief get*
    /// - All This method is used to get internal information.
    const std::string &get_config_file_path() const {return file_path_;};
    const IoSysConfig &get_led_config()             {return parameter_.led;}
    const IoSysConfig &get_beep_config()            {return parameter_.beep;}
    const DeviceSysConfig &get_key_config()         {return parameter_.key;}
    const SerialSysConfig &get_serial_config()      {return parameter_.serial;}
    const SocketSysConfig &get_tcp_config()         {return parameter_.tcp;}
    const SocketSysConfig &get_logger_config()      {return parameter_.logger;}
    const SocketSysConfig &get_internal_config()    {return parameter_.node;}
    const DeviceSysConfig &get_rtc_config()         {return parameter_.rtc;}
    const DeviceSysConfig &get_icm_config()         {return parameter_.icmSpi;}
    const DeviceSysConfig &get_ap_config()          {return parameter_.apI2c;}
    const JString &get_download_path()              {return parameter_.downloadpath;}
    const fw_information& get_fw_information()      {return parameter_.information;}
    
private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static system_config *instance_pointer_;

    /// \brief parameter_
    /// - object used to store the system parameter.
    SystemParamter parameter_;

    /// \brief file_path_
    /// - object used to store path of config file.
    std::string file_path_;
};
