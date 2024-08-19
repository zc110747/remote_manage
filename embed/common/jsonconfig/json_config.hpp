//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      json_config.hpp
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
_Pragma("once")

#include "json/json.h"

#define DEFAULT_LOCAL_IPADRESS      "127.0.0.1"
#define DEFAULT_IPADDRESS           "127.0.0.1"
#define DEFAULT_MQTT_PORT           1883
#define DEFAULT_FW_VERSION          "0.0.0.0"

#define DEFAULT_LED_INIT            0
#define DEFAULT_LED_DEV             "/dev/led"        
#define DEFAULT_BEEP_INIT           0
#define DEFAULT_BEEP_DEV            "/dev/beep"   
#define DEFAULT_KEY_DEV             "/dev/key"
#define DEFAULT_RTC_DEV             "/dev/rtc0"
#define DEFAULT_ICMSPI_DEV          "/dev/icm20608"
#define DEFAULT_API2C_DEV           "/dev/ap3216"
#define DEFAULT_VF610_DEV           "/sys/bus/iio/devices/iio:device0/in_voltage4_raw" 
#define DEFAULT_HX711_DEV           "/sys/bus/iio/devices/iio:device1/in_voltage1_raw"
#define DEFAULT_PWM_CHIP            "pwmchip0"
#define DEFAULT_PWM_STATE           0
#define DEFAULT_PWM_PEROID          0
#define DEFAULT_PWM_DUTY            0

#define DEFAULT_LOWER_DEVICE_LOGGER_PORT    15201
#define DEFAULT_LOWER_DEVICE_REMOTE_PORT    15202
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
#define DEFAULT_LOGGER_LEVEL        2

#define DEFAULT_NODE_PAGES          "/device/dist"

#define DEFAULT_LOGGER_PASSWD       "user123"
#define DEFAULT_ALLOW_NO_PASSWD     false

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
    JString vf610_adc_dev;
    JString hx711_dev;
}IIOSysConfig;

typedef struct 
{
    JString pwm_chip;
    int state;
    uint32_t peroid;
    uint32_t duty_cycle;
}PWMSysConfig;

typedef struct 
{
    DeviceSysConfig ap_i2c;
    DeviceSysConfig icm_spi;
    DeviceSysConfig key;
    DeviceSysConfig rtc;
    PWMSysConfig pwm;
    IoSysConfig led;
    IoSysConfig beep;
    IIOSysConfig iio;
}LocalDeviceConfig;

typedef struct 
{
    JString id;
    JString sub_topic;
    int keepalive;
    int qos;
}MqttDeivceInfo;

typedef struct 
{
    JString download_path;
    int node_port;
    int local_port;
    int gui_port;
    int logger_port;
    MqttDeivceInfo mqtt_device;
}MainProcessConfig;

typedef struct 
{
    int web_port;
    JString pages;
    MqttDeivceInfo mqtt_device;
}NodeServerConfig;

typedef struct 
{
    MqttDeivceInfo mqtt_device;    
}GuiManageConfig;

typedef struct 
{
    MqttDeivceInfo mqtt_device;    
}WinformConfig;

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
    int logger_port;
    
    int remote_port;

    SerialSysConfig serial;
}LowerDeviceConfig;

#define GUI_LOGGER_DEV          0
#define LOCAL_LOGGER_DEV        1
#define LOGGER_LOGGER_DEV       2
#define LOWER_LOGGER_DEV        3
#define MAIN_LOGGER_DEV         4
#define NODE_LOGGGE_DEV         5

typedef struct 
{
    int gui_manage_level;
    int local_device_level;
    int logger_device_level;
    int lower_device_level;
    int main_process_level;
    int node_server_level;
    JString passwd;
    bool allow_no_passwd;
}LoggerPrivilege;

typedef struct 
{
    JString local_ipaddress;
    
    JString ipaddress;

    JString mqtt_host;

    uint16_t mqtt_port;

    JString version;

    LocalDeviceConfig local_device;

    MainProcessConfig main_process;

    NodeServerConfig node_server;

    GuiManageConfig gui_manage;

    WinformConfig winform;

    LowerDeviceConfig lower_device;

    LoggerPrivilege logger_privilege;
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

    /// \brief save_configfile
    /// - This method is used to save config file.
    void save_configfile();

    /// \brief save_dynamicfile
    /// - This method is used to save dynamic file.
    void save_dynamicfile();

    /// \brief set_logger_level
    /// - This method is used to set logger level.
    bool set_logger_level(int dev, int level);

public:
    /// \brief get*
    /// - All This method is used to get internal information.
    const std::string &get_config_file_path()   const   {return file_path_;}
    const JString &get_local_ipaddress()        const   {return parameter_.local_ipaddress;}
    const JString &get_ipaddress()              const   {return parameter_.ipaddress;}
    const JString &get_mqtthost()               const   {return parameter_.mqtt_host;}
    const uint16_t get_mqttport()               const   {return parameter_.mqtt_port;}
    const JString &get_version()                const   {return parameter_.version;}

    const IoSysConfig &get_led_config()         const   {return parameter_.local_device.led;}
    const IoSysConfig &get_beep_config()        const   {return parameter_.local_device.beep;}
    const DeviceSysConfig &get_key_config()     const   {return parameter_.local_device.key;}
    const DeviceSysConfig &get_rtc_config()     const   {return parameter_.local_device.rtc;}
    const DeviceSysConfig &get_icm_config()     const   {return parameter_.local_device.icm_spi;}
    const DeviceSysConfig &get_ap_config()      const   {return parameter_.local_device.ap_i2c;}
    const IIOSysConfig &get_iio_config()        const   {return parameter_.local_device.iio;}
    const PWMSysConfig &get_pwm_config()        const   {return parameter_.local_device.pwm;}

    const int &get_lower_device_logger_port()   const    {return parameter_.lower_device.logger_port;}
    const int &get_lower_device_remote_port()   const    {return parameter_.lower_device.remote_port;}
    const SerialSysConfig &get_serial_config()  const    {return parameter_.lower_device.serial;}

    //main process
    const JString &get_download_path()          const    {return parameter_.main_process.download_path;}
    const int get_node_port()                   const    {return parameter_.main_process.node_port;}
    const int get_local_port()                  const    {return parameter_.main_process.local_port;}
    const int get_logger_port()                 const    {return parameter_.main_process.logger_port;}
    const int get_gui_device_port()             const    {return parameter_.main_process.gui_port;}
    const MqttDeivceInfo &get_mp_mqtt_config()     const    {return parameter_.main_process.mqtt_device;}

    //node info
    const int get_node_web_port()               const    {return parameter_.node_server.web_port;}
    const MqttDeivceInfo &get_node_mqtt_config()     const    {return parameter_.node_server.mqtt_device;}

    //gui info
    const MqttDeivceInfo &get_gui_mqtt_config() const    {return parameter_.gui_manage.mqtt_device; }

    //winform
    const MqttDeivceInfo &get_winform_mqtt_config() const    {return parameter_.winform.mqtt_device; }

    const LoggerPrivilege& get_logger_privilege() const  {return parameter_.logger_privilege;}

    const bool get_allow_no_passwd()            const   { return parameter_.logger_privilege.allow_no_passwd; }

private:
    /// \brief check_configfile
    /// - This method is used to check ipaddress wheather in list.
    bool check_configfile(const std::string& ipaddr); 

    /// \brief dynamic_init
    /// - This method is used to init dynamic information.
    /// \param path - path of the file for config.
    /// \return Wheather initialization is success or failed.
    bool dynamic_init(const char* path);

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

    /// \brief dynamic_path_
    /// - object used to store path of config file.
    std::string dynamic_path_;
};
