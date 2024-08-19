//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_manage.hpp
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

#include "driver.hpp"
#include "fifo_manage.hpp"

class device_manage final
{
public:
    /// \brief constructor
    device_manage() = default;
    device_manage(const device_manage&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~device_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static device_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_message
    /// - This method is used to send message for the device management.
    /// \param pEvent - the point of the event to send.
    /// \param size - the size of the event to send.
    /// \return nums of the message already send.
    int send_message(char* pEvent, int size);

private:
    /// \brief run
    /// - This method is used for thread run the device management.
    void run();

    /// \brief process_event
    /// - This method is used to process the event for device management.
    /// \param pEvent - the point of the event to process.
    void process_event(Event *pEvent);

    /// \brief update
    /// - This method is used for update the device info
    void update();

    /// \brief process_hardware
    /// - This method is used to process the hardware command.
    /// \param pEvent - the point of the event to process.
    void process_hardware(Event *pEvent);

    /// \brief process_sync
    /// - This method is used to process the sync command.
    /// \param pEvent - the point of the event to process.
    void process_sync(Event *pEvent);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static device_manage* instance_pointer_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_cmd_fifo_point_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_info_fifo_point_;

    /// \brief inter_info_
    /// - internal info used to store current device info.
    device_read_info inter_info_;

    /// \brief inter_info_
    /// - extend info used to compare and update the device info.
    device_read_info outer_info_;

    /// \brief rx_buffer_
    /// - buffer used to store rx command.
    char rx_buffer_[DEVICE_RX_BUFFER_SIZE];

    /// \brief tx_buffer_
    /// - buffer used to store tx info.
    char tx_buffer_[DEVICE_TX_BUFFER_SIZE];
};

#define ArraryToUint32(ptr) ((uint32_t)((*ptr)<<24 | (*(ptr+1))<<16 | (*(ptr+2))<<8 | (*(ptr+3))))