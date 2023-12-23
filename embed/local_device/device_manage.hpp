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

#define DEVICE_LOOP_EVENT               0
#define RX_BUFFER_SIZE                 512
#define TX_BUFFER_SIZE                 512

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

        if (memcmp((char *)this, (char *)&dev_info, size()) != 0)
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

    /// \brief copy_to_buffer
    /// - This method is used to convert internal data to buffer
    uint8_t copy_to_buffer(char *buffer) const
    {
        uint8_t size = 0;

        //header
        buffer[size++] = 0x5C;
        
        //len
        buffer[size++] = 0;

        buffer[size++] = led_io_;
        buffer[size++] = beep_io_;

        buffer[size++] = ap_info_.als>>8;
        buffer[size++] = ap_info_.als;
        buffer[size++] = ap_info_.ir>>8;
        buffer[size++] = ap_info_.ir;
        buffer[size++] = ap_info_.ps>>8;
        buffer[size++] = ap_info_.ps;

        int32_t data[8];
        data[0] = (int32_t)icm_info_.accel_x_act;
        data[1] = (int32_t)icm_info_.accel_y_act;
        data[2] = (int32_t)icm_info_.accel_z_act;
        data[3] = (int32_t)icm_info_.gyro_x_act;
        data[4] = (int32_t)icm_info_.gyro_y_act;
        data[5] = (int32_t)icm_info_.gyro_z_act;
        data[6] = (int32_t)icm_info_.temp_act;
        data[7] = (int32_t)angle_;

        for (int i=0; i<8; i++)
        {
            buffer[size++] = data[i]>>24;
            buffer[size++] = data[i]>>16;
            buffer[size++] = data[i]>>8;
            buffer[size++] = data[i];
        }

        //tail
        buffer[size++] = 0xC5;

        //update len， not include head, len, tail
        buffer[1] = size-3; 

        return size;
    }

    /// \brief size
    /// - This method is used to get the size of the object.
    /// \return the size of the object.
    size_t size()
    {
        return sizeof(*this);
    }
};

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

    /// \brief update
    /// - This method is used for update the device info
    void update();

private:
    /// \brief run
    /// - This method is used for thread run the device management.
    void run();

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

    /// \brief mutex_
    /// - mutex used to protect the update for outer_info_.
    std::mutex mutex_;

    /// \brief inter_info_
    /// - internal info used to store current device info.
    device_read_info inter_info_;

    /// \brief inter_info_
    /// - extend info used to compare and update the device info.
    device_read_info outer_info_;

    /// \brief rx_buffer
    /// - buffer used to store rx command.
    char rx_buffer[RX_BUFFER_SIZE];

    /// \brief rx_buffer
    /// - buffer used to store tx info.
    char tx_buffer[TX_BUFFER_SIZE];
};