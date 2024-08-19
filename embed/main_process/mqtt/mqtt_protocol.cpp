//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mqtt_protocol.cpp
//
//  Purpose:
//      mqtt receive decode
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "mqtt_protocol.hpp"
#include "device_process.hpp"

typedef enum
{
    mqGetStatus = 0,
    mqGetInfo,
    mqSetDevice,
    mqSetNet,
}mqtt_format_t;

const static std::map<std::string, mqtt_format_t> JsonCmdMap = {
    {"req_getstat", mqGetStatus},
    {"req_getinfo", mqGetInfo},
    {"req_setdev",  mqSetDevice},
    {"req_setnet",  mqSetNet}
};

const static std::map<std::string, int> JsonDevMap = {
    {"led",     DEVICE_LED},
    {"beep",    DEVICE_BEEP},
    {"pwm",     DEVICE_PWM}
};

const static std::map<std::string, int> JsonSourceMap = {
    {"SRC_NODE",        MQTT_SOURCE_NODE},
    {"SRC_GUI",         MQTT_SOURCE_GUI},
    {"SRC_DESKTOP",     MQTT_SOURCE_DESKTOP}
};

mqtt_protocol* mqtt_protocol::instance_pointer_ = nullptr;
mqtt_protocol* mqtt_protocol::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) mqtt_protocol();
        if (instance_pointer_ == nullptr)
        {
            PRINT_NOW("%s:mqtt_protocol new error!", PRINT_NOW_HEAD_STR);
        }
    }
    return instance_pointer_;
}

/*
{"command":"setdev", source:0, "device":"led", "data":"on"}
{"command":"setdev", source:1, "device":"pwm", "data":"1,50000,25000"}
*/
bool mqtt_protocol::decode_json_command(char *ptr, int size)
{
    try
    {
        Json::CharReaderBuilder readerBuilder;
        std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());
        Json::Value root;
        char const* begin = ptr;
        char const* end = begin + size; 
        JSONCPP_STRING errs;
        std::string data;
        mqtt_format_t format;
        int source;

        if(!reader->parse(begin, end, &root, &errs))
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "json reader parse failed");
            return false;
        }

        data = root["command"].asString();
        if (JsonCmdMap.count(data) == 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid commnd:%s", data.c_str());
            return false;
        } 
        format = JsonCmdMap.find(data)->second;
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "commnd:%s, index:%d", data.c_str(), format);
        
        data = root["source"].asString();
        if (JsonSourceMap.count(data) == 0)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid Source:%s", data.c_str());
            return false;
        } 
        source = JsonSourceMap.find(data)->second;

        switch(format)
        {
            case mqGetStatus:
                {
                    std::string&& device_info = device_process::get_instance()->get_dev_status();
                    mqtt_manage::get_instance()->mqtt_publish(get_topic(source), get_qos(source), device_info);
                }
                break;

            case mqGetInfo:
                {
                    std::string&& device_info = device_process::get_instance()->get_dev_info();
                    mqtt_manage::get_instance()->mqtt_publish(get_topic(source), get_qos(source), device_info);
                }
                break;

            case mqSetDevice:
                {
                    int dev;
                    data = root["device"].asString();
                    if (JsonDevMap.count(data) == 0)
                    {
                        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "invalid device:%s", data.c_str());
                        return false;
                    } 
                    dev = JsonDevMap.find(data)->second;
                    data = root["data"].asString();
                    if(cmd_process::get_instance()->process_device(dev, data))
                    {
                        mqtt_manage::get_instance()->mqtt_publish(get_topic(source), get_qos(source), "node");
                    }
                }
                break;
        }

        /* code */
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
}

const std::string& mqtt_protocol::get_topic(int source)
{
    switch(source)
    {
        case MQTT_SOURCE_GUI:
            return system_config::get_instance()->get_gui_mqtt_config().sub_topic;
        case MQTT_SOURCE_DESKTOP:
            return system_config::get_instance()->get_winform_mqtt_config().sub_topic;
        case MQTT_SOURCE_NODE:
        default:
            return system_config::get_instance()->get_node_mqtt_config().sub_topic;    
    }
}

const int mqtt_protocol::get_qos(int source)
{
    switch(source)
    {
        case MQTT_SOURCE_GUI:
            return system_config::get_instance()->get_gui_mqtt_config().qos;
        case MQTT_SOURCE_DESKTOP:
            return system_config::get_instance()->get_winform_mqtt_config().qos;
        case MQTT_SOURCE_NODE:
        default:
            return system_config::get_instance()->get_node_mqtt_config().qos;    
    }
}