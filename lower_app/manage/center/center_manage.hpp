//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      center_manage.cpp
//
//  Purpose:
//      核心管理模块，用于接收所有模块的数据，进行管理并分发
//      输入:网络通讯模块, QT界面事件，node输入事件, 设备状态更新，logger调试接口
//      输出：本地硬件设备
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

#include "modules.hpp"

class center_manage final
{
public:
    /// \brief constructor
    center_manage()=default;
    center_manage(const center_manage&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~center_manage()=delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static center_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_message
    /// - This method is used to send message for center manage.
    /// \param pMsg - event message pointer
    /// \param size - size of the event message
    /// \return Number of the message already send.
    int send_message(Event *pMsg, uint16_t size);

    /// \brief send_message
    /// - This method is used to send message for center manage.
    /// \param event_id - event id will be send.
    /// \param msg_pointer - start pointer for the data message
    /// \param size - data size for the message
    /// \return Number of the message already send.
    int send_message(uint16_t event_id, uint8_t *msg_pointer, uint16_t size);

    /// \brief send_hardware_update_message
    /// - This method is used to trigger the device hardware update to other process.
    /// \return Number of the message already send.
    int send_hardware_update_message();
    
    /// \brief send_hardware_config_message
    /// - This method is used to trigger the device hardware config message.
    /// \param device - the device need to process
    /// \param action - action of the device to process
    /// \return Number of the message already send.
    int send_hardware_config_message(uint8_t device, uint8_t action);

    /// \brief send_remote_device_status
    /// - This method is used to send remote device internal information.
    /// \param info - the info send to remote device
    void send_remote_device_status(const device_read_info &info);
    
private:
    /// \brief run
    /// - This method is used for thread run the center management.
    void run();

    /// \brief process_event
    /// - This method is used to process the event for center management.
    /// \param pEvent - the point of the event to process.
    /// \return wheather event process success or fail.
    bool process_event(Event *pEvent);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static inline center_manage* instance_pointer_ = nullptr;

    /// \brief center_fifo_point_
    /// - fifo point used for the center management.
    fifo_manage *center_fifo_point_{nullptr};
};