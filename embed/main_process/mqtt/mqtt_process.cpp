//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mqtt_process.cpp
//
//  Purpose:
//      mqtt client connect and translate process.
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "mqtt_process.hpp"

mqtt_device::mqtt_device(const mqtt_info &info, std::function<void(char *ptr, int size)> handler) 
:mosquittopp(info.id.c_str())
{
    info_ = info;
    func_handler_ = handler;
    memset(buffer_, 0, sizeof(buffer_));
};

mqtt_device::~mqtt_device()
{
}

void mqtt_device::mqtt_run()
{
    mosqpp::lib_init();

    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Mqtt start run!");
    connect(info_.host.c_str(), info_.port, info_.keepalive);

    while (1)
    {
        loop_forever();
    }

    mosqpp::lib_cleanup();
}

bool mqtt_device::start()
{
    thread_ = std::thread(std::bind(&mqtt_device::mqtt_run, this));
    thread_.detach();

    return true;
}

void mqtt_device::on_connect(int rc)
{
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "Connected with code:%d!", rc);
    if (rc == 0)
    {
        /* Only attempt to subscribe on a successful connect. */
        subscribe(NULL, info_.sub_topic.c_str(), info_.qos);
        is_connet_ = true;
    }
}

void mqtt_device::on_message(const struct mosquitto_message *message)
{
    if (!strcmp(message->topic, info_.sub_topic.c_str()))
    {
        /* Copy N-1 bytes to ensure always 0 terminated. */
        memcpy(buffer_, message->payload, message->payloadlen);
        buffer_[message->payloadlen] = '\0';
        if (func_handler_)
        {
            func_handler_(buffer_, message->payloadlen);
        }
    }
}

void mqtt_device::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
    PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "subscription topic:%s, succeeded!", info_.sub_topic.c_str());
}

int mqtt_device::publish_msg(const std::string &str)
{
    int ret = 0;

    if (is_connet_ && str.length() > 0)
    {
        ret = publish(NULL, info_.pub_topic.c_str(), str.length(), str.c_str(), info_.qos);
        PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "publisher, topic:%s, ret:%d!", info_.pub_topic.c_str(), ret);
    }
    
    return ret;
}

mqtt_manage* mqtt_manage::instance_pointer_ = nullptr;
mqtt_manage* mqtt_manage::get_instance()
{
    if (instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) mqtt_manage();
        if (instance_pointer_ == nullptr)
        {
            PRINT_NOW("%s:mqtt_manage new error!", PRINT_NOW_HEAD_STR);
        }
    }
    return instance_pointer_;
}

bool mqtt_manage::init()
{
    bool ret = false;

    try
    {
        const MqttDeivceInfo mqtt_config = system_config::get_instance()->get_mqtt_config();

        //mqtt subscribe init
        mqtt_info mqtt_process_info = {
            id:mqtt_config.id,
            host:system_config::get_instance()->get_ipaddress(),
            port:mqtt_config.port,
            sub_topic:mqtt_config.sub_topic,
            pub_topic:mqtt_config.pub_topic,
            keepalive:mqtt_config.keepalive,
            qos:mqtt_config.qos
        };

        mqtt_device_ptr = std::make_unique<mqtt_device>(mqtt_process_info, [](char *ptr, int size){
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "rx_buffer_size:%d.", size);
        });
        if (mqtt_device_ptr == nullptr)
        {
            PRINT_NOW("%s:mqtt device new failed!", PRINT_NOW_HEAD_STR);
            return false;
        }
        ret = mqtt_device_ptr->start();
    }
    catch(const std::exception& e)
    {
        PRINT_NOW("%s:mqtt init error:%s!", PRINT_NOW_HEAD_STR, e.what());
    }
    return ret;
}

int mqtt_manage::mqtt_publish(const std::string &str)
{
    int ret = -1;

    if (mqtt_device_ptr != nullptr)
    {
        ret = mqtt_device_ptr->publish_msg(str);
    }
    return ret;
}
