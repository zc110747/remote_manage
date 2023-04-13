//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      device_manage.hpp
//
//  Purpose:
//   	进行设备的管理，周期性的读取硬件信息, 并能够处理外部事件来控制硬件
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
#include "driver.hpp"
#include <type_traits>
#include <cstring>
#include "event.hpp"

//interal event
//device event
#define DEVICE_ID_TIME_UPDATE_PREOID    0x0001
#define DEVICE_ID_HARDWARE_CHANGE       0x0002

//workflow event
#define WORKFLOW_ID_HARDWARE_UPDATE  0x1001    //更新内部数据
#define WORKFLOW_ID_HARDWARE_CHANGE     0x1002    //硬件数据更新

//process hardware chage
#define EVENT_DEVICE_LED                0x00
#define EVENT_DEVICE_BEEP               0x01
#define READ_BUFFER_SIZE                1024

namespace NAMESPACE_DEVICE
{
    struct device_read_info
    {
        /// \brief led_io_
        /// - led i/o status.
        uint8_t   led_io_;

        /// \brief beep_io_
        /// - beep i/o status.
        uint8_t   beep_io_; 

        /// \brief ap_info_
        /// - ap3216 information.
        ap_info   ap_info_;

        /// \brief icm_info_
        /// - icm20608 information.
        icm_info  icm_info_;

        /// \brief angle_
        /// - angle value.
        int32_t   angle_;

        /// \brief operator !=
        /// - This method is used to compare the two object.
        /// \param dev_info - current object will compare.
        /// \return if equal return true, else false.
        bool operator != (const device_read_info& dev_info)
        {
            static_assert(std::is_trivial_v<device_read_info>, "Not Allow C memory process!");

            if(memcmp((char *)this, (char *)&dev_info, size()) != 0)
                return true;
            return false;
        }

        /// \brief clear
        /// - This method is used to reset the object.
        void clear()
        {
            static_assert(std::is_trivial_v<device_read_info>, "Not Allow C memory process!");

            memset((char *)this, 0, size());
        }

        /// \brief size
        /// - This method is used to get the size of the object.
        /// \return the size of the object.
        size_t size()
        {
            return sizeof(*this);
        }
    };

    class device_manage
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

        /// \brief get_device_info
        /// - get the infomation of the device, for other thread, need copy.
        /// \return the information of the device.
        device_read_info get_device_info();

        /// \brief send_message
        /// - This method is used to send message for the device management.
        /// \param pEvent - the point of the event to send.
        /// \param size - the size of the event to send.
        /// \return nums of the message already send.
        int send_message(char* pEvent, int size);

        /// \brief send_device_message
        /// - This method is used to send device message.
        /// \param device - the device will manage.
        /// \param action - the device action.
        /// \return nums of the message already send.
        int send_device_message(uint8_t device, uint8_t action);

    private:
        /// \brief run
        /// - This method is used for thread run the device management.
        void run();

        /// \brief process_event
        /// - This method is used to process the event for device management.
        /// \param pEvent - the point of the event to process.
        /// \return wheather event process success or fail.
        bool process_event(Event *pEvent);

        /// \brief update
        /// - This method is used to update the internal information of the device.
        void update();

        /// \brief process_hardware
        /// - This method is used to process the hardware command.
        /// \param pEvent - the point of the event to process.
        void process_hardware(Event *pEvent);

    private:
        /// \brief instance_pointer_
        /// - object used to implement the singleton pattern.
        static device_manage* instance_pointer_;

        /// \brief inter_info_
        /// - internal info used to store current device info.
        device_read_info inter_info_;

        /// \brief inter_info_
        /// - extend info used to compare and update the device info.
        device_read_info outer_info_;

        /// \brief mutex_
        /// - mutex used to protect the update for outer_info_.
        std::mutex mutex_;

        /// \brief device_fifo_point_
        /// - fifo point used for the device management.
        fifo_manage *device_fifo_point_{nullptr};
    };
}
