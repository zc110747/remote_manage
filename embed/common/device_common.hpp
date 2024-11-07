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
#include "logger_manage.hpp"
#include "time_manage.hpp"

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

#define STRUCT_STRING_MAX_SIZE  128

typedef struct
{
    uint8_t size;
    char buffer[STRUCT_STRING_MAX_SIZE];
}STRUCT_STRING;

struct device_read_info
{
    /// \brief led_io_
    /// - led i/o status.
    uint8_t   led_io_;

    /// \brief beep_io_
    /// - beep i/o status.
    uint8_t   beep_io_; 
    
    /// \brief pwm_duty_
    /// - current pwm duty. 
    uint8_t   pwm_duty_;

    /// \brief ap_info_
    /// - ap3216 information.
    ap_info   ap_info_;

    /// \brief icm_info_
    /// - icm20608 information.
    icm_info  icm_info_;

    /// \brief angle_
    /// - angle value.
    int32_t   angle_;

    /// \brief hx711_
    /// - hx711 value.
    uint32_t hx711_;

    /// \brief vf610_adc_
    /// - vf610 value.
    float vf610_adc_;

    /// \brief rtc_time
    /// - rtc time value.
    STRUCT_STRING rtc_timer;

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
    /// \param buffer - buffer used to save the convert data
    /// \return the convert buffer size
    uint8_t copy_to_buffer(char *buffer) const
    {
        uint8_t size = 0;
        UNION_FLOAT_INT data[10];

        buffer[size++] = led_io_;
        buffer[size++] = beep_io_;
        buffer[size++] = pwm_duty_;
        buffer[size++] = ap_info_.als>>8;
        buffer[size++] = ap_info_.als;
        buffer[size++] = ap_info_.ir>>8;
        buffer[size++] = ap_info_.ir;
        buffer[size++] = ap_info_.ps>>8;
        buffer[size++] = ap_info_.ps;

        data[0].f_val = icm_info_.accel_x_act;
        data[1].f_val = icm_info_.accel_y_act;
        data[2].f_val = icm_info_.accel_z_act;
        data[3].f_val = icm_info_.gyro_x_act;
        data[4].f_val = icm_info_.gyro_y_act;
        data[5].f_val = icm_info_.gyro_z_act;
        data[6].f_val = icm_info_.temp_act;
        data[7].f_val = angle_;
        data[8].i_val = (uint32_t)hx711_;
        data[9].f_val = vf610_adc_;

        for (int i=0; i<10; i++)
        {
            memcpy(&buffer[size], data[i].buffer, 4);
            size += 4;
        }

        buffer[size++] = rtc_timer.size; 
        if (rtc_timer.size > 0)
        {
            memcpy(&buffer[size], rtc_timer.buffer, rtc_timer.size);
            size += rtc_timer.size;
        }

        return size;
    }

    /// \brief copy_to_device
    /// - This method is used to convert buffer to internal data
    /// \param buffer - the buffer used for convert
    void copy_to_device(char *buffer)
    {
        uint8_t size = 0;
        UNION_FLOAT_INT data;

        led_io_ = buffer[size++];
        beep_io_ = buffer[size++];
        pwm_duty_ = buffer[size++];
        
        ap_info_.als = CREATE_UINT16(buffer[size], buffer[size+1]);
        size += 2;
        ap_info_.ir = CREATE_UINT16(buffer[size], buffer[size+1]);
        size += 2;
        ap_info_.ps = CREATE_UINT16(buffer[size], buffer[size+1]);
        size += 2;

        memcpy(data.buffer, &buffer[size], 4);
        icm_info_.accel_x_act = data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);       
        icm_info_.accel_y_act = data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);        
        icm_info_.accel_z_act = data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        icm_info_.gyro_x_act =  data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        icm_info_.gyro_y_act =  data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);
        icm_info_.gyro_z_act =  data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        icm_info_.temp_act =    data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        angle_ =                data.f_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        hx711_ =                data.i_val;
        size += 4;

        memcpy(data.buffer, &buffer[size], 4);  
        vf610_adc_ =            data.f_val;  
        size += 4;

        rtc_timer.size = buffer[size++];
        if (rtc_timer.size > 0)
        {
            memcpy(rtc_timer.buffer, &buffer[size], rtc_timer.size);
        }
        PRINT_LOG(LOG_DEBUG, xGetCurrentTimes(), "data:%d,%f", hx711_, icm_info_.gyro_y_act);
    }

    /// \brief size
    /// - This method is used to get the size of the object.
    /// \return the size of the object.
    size_t size()
    {
        return sizeof(*this);
    }
};
