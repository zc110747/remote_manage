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
#define WORKFLOW_ID_INTER_DATA_REFRESH  0x1001    //更新内部数据
#define WORKFLOW_ID_HARDWARE_CHANGE     0x1002    //硬件数据更新

//process hardware chage
#define EVENT_DEVICE_LED                0x00
#define EVENT_DEVICE_BEEP               0x01
#define READ_BUFFER_SIZE                1024

namespace NAMESPACE_DEVICE
{
    struct device_read_info
    {
        uint8_t   led_io_;
        uint8_t   beep_io_; 
        ap_info   ap_info_;
        icm_info  icm_info_;
        int32_t   angle_;

        //for compare, need clear before to avoid fill by align.
        bool operator != (const device_read_info& dev_info)
        {
            static_assert(std::is_trivial_v<device_read_info>, "Not Allow C memory process!");

            if(memcmp((char *)this, (char *)&dev_info, size()) != 0)
                return true;
            return false;
        }

        void clear()
        {
            static_assert(std::is_trivial_v<device_read_info>, "Not Allow C memory process!");

            memset((char *)this, 0, size());
        }

        size_t size()
        {
            return sizeof(*this);
        }
    };

    class device_manage
    {
    public:
    	/// - constructor.
        device_manage() = default;

        /// - destructor, delete not allow for p
        virtual ~device_manage() = delete;

        /// \brief getInstance
        /// - This method is used to get the pattern of the class.
        /// \return the singleton pattern point of the object.
        static device_manage* getInstance();

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
        /// \param action - the device action .
        /// \return nums of the message already send.
        int send_device_message(uint8_t device, uint8_t action);

    private:
        device_read_info inter_info;
        device_read_info outer_info;
        std::mutex mut;
        static device_manage* pInstance;
        fifo_manage *pDevFIFO{nullptr};

        void run();
        bool EventProcess(Event *pEvent);

        void update();
        void HardProcess(Event *pEvent);


    };
}
