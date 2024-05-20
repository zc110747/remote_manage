//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      i2c_ap.hpp
//
//  Purpose:
//      AP模块对应的I2C接口，通过继承支持直接读取模块信息
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

#include "common_unit.hpp"
#include "device_common.hpp"

class iio_device
{
public:
    /// \brief constructor
    iio_device(){

    }

    /// -- destructor
    ~iio_device(){

    }

    /// \brief init
    /// - init the device.
    /// \param DevicePath - path of the system device.
    /// \param flags - flags process the device.
    /// \return Wheather initialize is success or failed.
    virtual bool init(const std::string &DevicePath)
    {
        device_path_ = DevicePath;
        file_ = fopen(device_path_.c_str(), "r");
        if(file_ == nullptr)
        {
            return false;
        }
        return true;
    }

    /// \brief release
    /// - release the device.   
    virtual void release()
    {
        if(file_ != nullptr)
        {
            fclose(file_);
            file_ = nullptr;
        }
        device_path_.clear();
    }

    /// \brief read_string
    /// - read internal string
    /// \param str - memory store the string.
    /// \param size - max_size for read.
    /// \return success is the lenght, <=0 means failed
    virtual int read_string(char *str, size_t size)
    {
        int ret;

        if(file_ == nullptr)
        {
            return -1;
        }
            
        ret = fread(str, 1, size, file_);
        if(ret <= 0) 
        {
            printf("file read error!\r\n");
            return -1;
        }
        fseek(file_, 0, SEEK_SET);
        return ret;
    }

private:
    /// \brief device_path_
    /// - the device path in system
    std::string device_path_;

    /// \brief file_
    /// - file process stream
    FILE *file_{nullptr};
};
