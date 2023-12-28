//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_common.hpp
//
//  Purpose:
//      global device infomation.
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

#include <stdint.h>
#include <cstring>
#include <type_traits>
#include "common.hpp"

typedef struct _SApInfo
{
    uint16_t ir;     /*红外线强度*/
    uint16_t als;    /*环境光强度*/
    uint16_t ps;     /*接近距离*/
}ap_info;

typedef struct
{
    int gyro_x_adc;     /*陀螺仪x轴角速度*/
    int gyro_y_adc;     /*陀螺仪y轴角速度*/
    int gyro_z_adc;     /*陀螺仪z轴角速度*/ 
    int accel_x_adc;    /*加速度计x轴加速度*/
    int accel_y_adc;    /*加速度计y轴加速度*/
    int accel_z_adc;    /*加速度计z轴加速度*/
    int temp_adc;       /*温度信息*/
}icm_adc_info;

typedef struct
{
    float gyro_x_act;     /*陀螺仪x轴角速度*/
    float gyro_y_act;     /*陀螺仪y轴角速度*/
    float gyro_z_act;     /*陀螺仪z轴角速度*/ 
    float accel_x_act;    /*加速度计x轴加速度*/
    float accel_y_act;    /*加速度计y轴加速度*/
    float accel_z_act;    /*加速度计z轴加速度*/     
    float temp_act;       /*温度信息*/
}icm_info;

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

        return size;
    }

    void copy_to_device(char *buffer)
    {
        led_io_ = buffer[0];
        beep_io_ = buffer[1];

        ap_info_.als = CREATE_UINT16(buffer[2], buffer[3]);
        ap_info_.ir = CREATE_UINT16(buffer[4], buffer[5]);
        ap_info_.ps = CREATE_UINT16(buffer[6], buffer[7]);

        icm_info_.accel_x_act = CREATE_FLOAT(buffer[8], buffer[9], buffer[10], buffer[11]);
        icm_info_.accel_y_act = CREATE_FLOAT(buffer[12], buffer[13], buffer[14], buffer[15]);
        icm_info_.accel_z_act = CREATE_FLOAT(buffer[16], buffer[17], buffer[18], buffer[19]);
        icm_info_.gyro_x_act =  CREATE_FLOAT(buffer[20], buffer[21], buffer[22], buffer[23]);
        icm_info_.gyro_y_act =  CREATE_FLOAT(buffer[24], buffer[25], buffer[26], buffer[27]);
        icm_info_.gyro_z_act =  CREATE_FLOAT(buffer[28], buffer[29], buffer[30], buffer[31]);
        icm_info_.temp_act =    CREATE_FLOAT(buffer[32], buffer[33], buffer[34], buffer[35]);
        angle_ =                CREATE_FLOAT(buffer[36], buffer[37], buffer[38], buffer[39]);
    }

    /// \brief size
    /// - This method is used to get the size of the object.
    /// \return the size of the object.
    size_t size()
    {
        return sizeof(*this);
    }
};
