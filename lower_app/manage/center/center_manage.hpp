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
    /// - constructor.
    center_manage()=default;
    center_manage(const center_manage&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~center_manage()=delete;

    /// \brief getInstance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static center_manage* getInstance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_message
    /// - This method is used to send message for center manage.
    /// \return Number of the message already send.
    int send_message(Event *pMsg, uint16_t size);

    /// \brief send_hardware_update_message
    /// - This method is used to trigger the device hardware update to other process.
    /// \return Number of the message already send.
    int send_hardware_update_message();
    
    /// \brief send_hardware_config_message
    /// - This method is used to trigger the device hardware config message.
    /// \return Number of the message already send.
    int send_hardware_config_message(uint8_t device, uint8_t action);

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
    /// \brief pInstance
    /// - object used to implement the singleton pattern.
    static inline center_manage* pInstance = nullptr;

    /// \brief center_fifo_point_
    /// - fifo point used for the center management.
    fifo_manage *center_fifo_point_{nullptr};
};