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

#define DEFAULT_LOCAL_IPADRESS      "127.0.0.1"
#define DEFAULT_IPADDRESS           "127.0.0.1"
#define DEFAULT_FW_VERSION          "0.0.0.0"

#define DEFAULT_LED_INIT            0
#define DEFAULT_LED_DEV             "/dev/led"        
#define DEFAULT_BEEP_INIT           0
#define DEFAULT_BEEP_DEV            "/dev/beep"   
#define DEFAULT_KEY_DEV             "/dev/key"
#define DEFAULT_RTC_DEV             "/dev/rtc0"
#define DEFAULT_ICMSPI_DEV          "/dev/icm20608"
#define DEFAULT_API2C_DEV           "/dev/ap3216"
#define DEFAULT_SERIAL_BAUD         115200
#define DEFAULT_SERIAL_DATABITS     8
#define DEFAULT_SERIAL_STOPBITS     1
#define DEFAULT_SERIAL_PARITY       "n"
#define DEFAULT_SERIAL_DEV          "/dev/ttymxc2"

#define DEFAULT_DOWNLOAD_PATH       "/tmp/download/"
#define DEFAULT_NODE_PORT           8000
#define DEFAULT_LOWER_DEVICE_PORT   8001
#define DEFAULT_LOCAL_DEVICE_PORT   8002
#define DEFAULT_LOGGER_PORT         8003
#define DEFAULT_GUI_DEVICE_PORT     8004
#define DEFAULT_NODE_WEB_PORT       8100


using JString = Json::String;

typedef struct 
{
    JString dev;
}DeviceSysConfig;

typedef struct 
{
    int init;
    JString dev;
}IoSysConfig;

typedef struct 
{
    DeviceSysConfig ap_i2c;
    DeviceSysConfig icm_spi;
    DeviceSysConfig key;
    DeviceSysConfig rtc;
    IoSysConfig led;
    IoSysConfig beep;
}LocalDeviceConfig;


typedef struct 
{
    JString download_path;
    int node_port;
    int local_port;
    int gui_port;
    int logger_port;
}MainProcessConfig;

typedef struct 
{
    int web_port;
}NodeServerConfig;

typedef struct 
{
    int net_port;
    int baud;
    int dataBits;
    int stopBits;
    JString parity;
    JString dev;
}SerialSysConfig;

typedef struct 
{
    int logger_port;
    
    SerialSysConfig serial;
}LowerDeviceConfig;

typedef struct 
{
    JString local_ipaddress;
    
    JString ipaddress;

    JString version;

    LocalDeviceConfig local_device;

    MainProcessConfig main_process;

    NodeServerConfig node_sever;

    LowerDeviceConfig lower_device;
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

    /// \brief default_init
    /// - This method is used to set all data default.
    void default_init() noexcept;

    /// \brief save_config_file
    /// - This method is used to save config file.
    void save_config_file();

public:
    /// \brief get*
    /// - All This method is used to get internal information.
    const std::string &get_config_file_path()   const   {return file_path_;}
    const JString &get_local_ipaddress()        const   {return parameter_.local_ipaddress;}
    const JString &get_ipaddress()              const   {return parameter_.ipaddress;}
    const JString &get_version()                const   {return parameter_.version;}

    const IoSysConfig &get_led_config()         const   {return parameter_.local_device.led;}
    const IoSysConfig &get_beep_config()        const   {return parameter_.local_device.beep;}
    const DeviceSysConfig &get_key_config()     const   {return parameter_.local_device.key;}
    const DeviceSysConfig &get_rtc_config()     const   {return parameter_.local_device.rtc;}
    const DeviceSysConfig &get_icm_config()     const   {return parameter_.local_device.icm_spi;}
    const DeviceSysConfig &get_ap_config()      const   {return parameter_.local_device.ap_i2c;}

    const int &get_lower_device_logger_port()   const    {return parameter_.lower_device.logger_port;}
    const SerialSysConfig &get_serial_config()  const    {return parameter_.lower_device.serial;}

    const JString &get_download_path()          const    {return parameter_.main_process.download_path;}
    const int get_node_port()                   const    {return parameter_.main_process.node_port;}
    const int get_local_port()                  const    {return parameter_.main_process.local_port;}
    const int get_logger_port()                 const    {return parameter_.main_process.logger_port;}   
    const int get_gui_device_port()             const    {return parameter_.main_process.gui_port;}
    const int get_node_web_port()               const    {return parameter_.node_sever.web_port;}
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
