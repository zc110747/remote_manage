//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_base.hpp
//
//  Purpose:
//      封装的设备底层接口，基类为device_base, 支持open/close
//      io_base主要封装IO支持的接口, open, read, write, close
//      info_base主要封装支持特定外设的接口, 可以读取内部信息
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

#include "json_config.hpp"
#include "common_unit.hpp"
#include "device_common.hpp"

class device_base
{
public:
    /// \brief constructor
    device_base();

    /// -- destructor
    ~device_base();

    /// \brief init
    /// - init the device.
    /// \param DevicePath - path of the system device.
    /// \param flags - flags process the device.
    /// \return Wheather initialize is success or failed.
    virtual bool init(const std::string &DevicePath, int flags);

    /// \brief getfd
    /// - get current device fd.
    /// \return the device fd of current device.
    int getfd() { return device_fd_; }

    /// \brief getDevicePath
    /// - get current device path.
    /// \return the path of current device.
    const std::string &getDevicePath() {return device_path_;}

protected:
    /// \brief open
    /// - open the device.
    /// \return Wheather open is success or failed.
    virtual bool open(int flags);

    /// \brief close
    /// - close the device.
    virtual void close();

    /// \brief device_fd_
    /// - the device id when open the device.
    int device_fd_;

    /// \brief device_path_
    /// - the device path in system
    std::string device_path_;
};

class io_base:public device_base
{
public:
    /// \brief constructor
    using device_base::device_base;

    /// \brief read_io_status
    /// - read io status.
    /// \return wheather read is success or fail.
    bool read_io_status();

    /// \brief write_io_status
    /// - write io status.
    /// \param status - the write io status.
    /// \return wheather write is success or fail 
    bool write_io_status(uint8_t status);

    /// \brief on
    /// - set the i/o status on.
    /// \return wheather set is success or fail 
    bool on();

    /// \brief off
    /// - set the i/o status off.
    /// \return wheather set is success or fail 
    bool off();

    /// \brief trigger
    /// - reserved the i/o status.
    /// \return wheather reserved is success or fail 
    bool trigger();
    
    /// \brief get_io_status
    /// - return the current i/o status.
    /// \return current i/o status
    uint8_t get_io_status()  {return status_;}

private:
    /// \brief status_
    /// - The io work status.
    std::atomic<uint8_t> status_{0};
};

#define MAX_INFO_SIZE   48
template<typename T>
class info_base:public device_base
{
public:
    /// \brief constructor
    using device_base::device_base;

    /// \brief read_info
    /// - read the device information.
    /// \return wheather read information is success or fail 
    bool read_info()
    {
        bool ret = false;
        ssize_t nSize;

        if (device_fd_ >= 0)
        {
            int readSize = sizeof(data.info);
            
            readSize = readSize<MAX_INFO_SIZE?readSize:MAX_INFO_SIZE;
            nSize = ::read(device_fd_, data.buffer, readSize);
            if (nSize > 0)
            {
                ret = true;
            }
        }
        return ret;
    }

    /// \brief get_info
    /// - get the device information.
    /// \return the device infomation
    T get_info() {return data.info;}

protected:
    /// \brief data
    /// - The information of the device.
    union {
        char buffer[MAX_INFO_SIZE];
        T info;
    } data;
};
