//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      cmd_process.hpp
//
//  Purpose:
//      command process for local device.
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
#include "common_unit.hpp"

typedef enum
{
    cmdSetLevel = 0,
    cmdGetStatus,
    cmdSetDevice,
    cmdGetDevice,
    cmdPublish,
    cmdGetHelp,
    cmdGetOS,
}cmd_format_t;

class cmd_process
{
public:
    /// \brief constructor
    cmd_process() = default;
    cmd_process(const cmd_process&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~cmd_process() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static cmd_process* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief process_data
    /// - This method is used to process device.
    /// \return Wheather process is success or failed.
    bool process_device(int dev, const std::string& data);
    
private:
    /// \brief run
    /// - This method is used for thread run the device management.
    void run();

    /// \brief parse_data
    /// - This method is used to parse the receive data and save format.
    /// \return Wheather parse is success or failed.
    bool parse_data();

    /// \brief process_data
    /// - This method is used to process the data already parse.
    /// \return Wheather process is success or failed.
    bool process_data();
    
    /// \brief show_os
    /// - This method is show current os information.
    void show_os();

    /// \brief sync_level
    /// - This method is used to sync level.
    void sync_level(int dev, int level);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static cmd_process* instance_pointer_;

    /// \brief rx_buffer_
    /// - buffer used to store rx command.
    char rx_buffer_[DEVICE_RX_BUFFER_SIZE];

    /// \brief rx_size_
    /// - buffer rx size.
    int rx_size_;

    /// \brief cmd_format_
    /// - command alread parse from the data receive.
    cmd_format_t cmd_format_;

    /// \brief cmd_data_pointer_
    /// - memory point the start to data area for received.
    char *cmd_data_pointer_;

    /// \brief cmd_process_thread_
    /// - cmd process thread object.
    std::thread cmd_process_thread_;

    /// \brief logger_main_process_tx_fifo_
    /// - fifo used for logger server rx and write to local device.
    std::unique_ptr<fifo_manage> logger_main_process_tx_fifo_{nullptr};
};
