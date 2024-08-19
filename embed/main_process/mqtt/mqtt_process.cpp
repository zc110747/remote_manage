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
#include "cmd_process.hpp"
#include "mqtt_protocol.hpp"

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

int mqtt_device::publish_msg(const std::string &topic, int qos, const char* ptr, int size)
{
    int ret = 0;

    if (is_connet_ && size > 0)
    {
        ret = publish(NULL, topic.c_str(), size, ptr, qos);
        //PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "publisher, topic:%s, ret:%d!", info_.pub_topic.c_str(), ret);
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

//"A0 + json data"
//"0x41 0x31 + Hex data"
bool mqtt_manage::init()
{
    bool ret = false;

    try
    {
        const MqttDeivceInfo mqtt_config = system_config::get_instance()->get_mp_mqtt_config();

        //mqtt subscribe init
        mqtt_info mqtt_process_info = {
            id:mqtt_config.id,
            host:system_config::get_instance()->get_mqtthost(),
            port:system_config::get_instance()->get_mqttport(),
            sub_topic:mqtt_config.sub_topic,
            keepalive:mqtt_config.keepalive,
            qos:mqtt_config.qos
        };

        mqtt_device_ptr = std::make_unique<mqtt_device>(mqtt_process_info, [](char *ptr, int size)
        {
            PRINT_LOG(LOG_INFO, xGetCurrentTimes(), "rx_buffer_size:%d, buffer:%s.", size, ptr);

            if(size > 2)
            {
                if (memcmp(ptr, JSON_DATA_HEAD, 2) == 0)
                {
                    mqtt_protocol::get_instance()->decode_json_command(ptr+2, size);
                }
                else if(memcmp(ptr, HEX_DATA_HEAD, 2) == 0)
                {
                    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "hex data receive!");
                }
            }
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

int mqtt_manage::mqtt_publish(const std::string &topic, int qos, const std::string &str)
{
    int ret = -1;

    if (mqtt_device_ptr != nullptr)
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "topic:%s, data:%s", topic.c_str(), str.c_str());
        ret = mqtt_device_ptr->publish_msg(topic, qos, str.c_str(), str.size());
    }
    return ret;
}

int mqtt_manage::mqtt_publish(const std::string &topic, int qos, char *ptr, uint16_t size)
{
    int ret = -1;

    if (mqtt_device_ptr != nullptr)
    {
        PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "topic:%s, data:%s", topic.c_str(), ptr);
        ret = mqtt_device_ptr->publish_msg(topic, qos, ptr, size);
    }
    return ret;   
}