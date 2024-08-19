//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      mqtt_protocol.hpp
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
_Pragma("once")

#include "mqtt_process.hpp"
#include "cmd_process.hpp"

class mqtt_protocol
{
public:
    /// \brief constructor
    mqtt_protocol() = default;
    mqtt_protocol(const mqtt_protocol&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~mqtt_protocol() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static mqtt_protocol* get_instance();

    /// \brief parse_data
    /// - This method is used to decode command received.
    /// \return Wheather parse is success or failed.
    bool decode_json_command(char *ptr, int size);

private:
    const std::string& get_topic(int source);

    const int get_qos(int source);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static mqtt_protocol* instance_pointer_;
};