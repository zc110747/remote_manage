//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mqtt_process.hpp
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
_Pragma("once")

#include <mosquittopp.h>
#include "common_unit.hpp"

typedef struct
{
    std::string id;
    std::string host;
    int port;
    std::string sub_topic;
    std::string pub_topic;
    int keepalive{60};
    int qos{1};
}mqtt_info;

class mqtt_device : public mosqpp::mosquittopp
{
public:
    /// \brief constructor
    /// \param info -- info used to initialize the mqtt service.
    mqtt_device(const mqtt_info &info, std::function<void(char *ptr, int size)> handler);

    /// \brief destructor
    ~mqtt_device();

    /// \brief on_connect
    /// - This method is used do mqtt connect
    /// \param rc -- state for mqtt connect, 0 is success.
    void on_connect(int rc);

    /// \brief on_message
    /// - This method is used do mqtt message receive
    /// \param message -- memssage when receive from subscribe topic
    void on_message(const struct mosquitto_message *message);

    /// \brief on_subscribe
    /// - This method is used do subscribe success.
    /// \param mid --
    /// \param qos_count --
    /// \param granted_qos --
    void on_subscribe(int mid, int qos_count, const int *granted_qos);

    /// \brief publish_msg
    /// - This method is publish msg str.
    /// \param str -- memssage when receive by subscribe
    /// \return publish msg process.
    int publish_msg(const std::string &str);

    /// \brief start
    /// - This method is used to start mqtt process.
    bool start();

private:
    /// \brief mqtt_run
    /// - mqtt loop run thread.
    void mqtt_run();

private:
    /// \brief info_
    /// - info used to store mqtt config.
    mqtt_info info_;

    /// \brief is_connet_
    /// - wheather mqtt success connect.
    bool is_connet_{false};

    /// \brief thread_
    /// - mqtt run thread object.
    std::thread thread_;

    /// \brief buffer_
    /// - buffer store subscription information.
    char buffer_[512];

    /// \brief buffer_
    /// - buffer store subscription information.
    std::function<void(char *ptr, int size)> func_handler_;
};

class mqtt_manage
{
public:
    /// \brief constructor
    mqtt_manage() = default;
    mqtt_manage(const mqtt_manage&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~mqtt_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static mqtt_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief init
    /// - This method is used to publisher string.
    /// \param str -- memssage publisher the string.
    /// \return Wheather initialization is success or failed.
    int mqtt_publish(const std::string &str);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static mqtt_manage* instance_pointer_;

    /// \brief mqtt_device_ptr
    /// - mqtt device manage pointer.
    std::unique_ptr<mqtt_device> mqtt_device_ptr;
};

bool mqtt_init(void);
int mqtt_publish(const std::string &str);