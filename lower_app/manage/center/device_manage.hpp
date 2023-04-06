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
        uint8_t   led_io;
        uint8_t   beep_io; 
        AP_INFO   ap_info;
        ICM_INFO  icm_info;
        int32_t   angle;

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

    public:
        device_manage() = default;
        virtual ~device_manage() = delete; //单例模式不允许删除 

        bool init();
        device_read_info getDeviceInfo();
        static device_manage* getInstance();

        
    public:
        int sendMessage(char* pEvent, int size);
        int sendHardProcessMsg(uint8_t device, uint8_t action);
    };
}
