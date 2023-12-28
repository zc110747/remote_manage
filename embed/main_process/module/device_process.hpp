//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_process.hpp
//
//  Purpose:
//      包含应用配置信息的文件
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

#include "fifo_manage.hpp"
#include "device_common.hpp"

class device_process final
{
public:
    /// \brief constructor
    device_process() = default;
    device_process(const device_process&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~device_process() = delete;
    
    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static device_process* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_buffer
    /// - This method is used to sendbuffer to local device
    /// \param ptr - point to send buffer
    /// \param size - size of the send buffer
    /// \return buffer send size. 
    int send_buffer(const char* ptr, int size);

    /// \brief update_device_string
    /// - update device string info.
    void update_info_string();

private:
    /// \brief run
    /// - This method is used for thread run the device management.
    void run();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static device_process* instance_pointer_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_cmd_fifo_point_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_info_fifo_point_;

    /// \brief rx_buffer_
    /// - buffer used to store rx command.
    char rx_buffer_[DEVICE_RX_BUFFER_SIZE];

    /// \brief tx_buffer_
    /// - buffer used to store tx info.
    char tx_buffer_[DEVICE_TX_BUFFER_SIZE];

    /// \brief tx_buffer_
    /// - buffer used to store tx info.    
    device_read_info info_;

    /// \brief info_str_
    /// - the info string buffer
    std::string info_str_;
};