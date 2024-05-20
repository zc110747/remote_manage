//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      device_process.hpp
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

#include "fifo_manage.hpp"
#include "device_common.hpp"

struct local_sysinfo final
{
    std::string cpu_info;

    std::string kernel_info;

    //MB
    uint32_t disk_total;

    uint32_t disk_used;

    //MB
    uint32_t ram_total;

    uint32_t ram_used;

    local_sysinfo()
    {
        cpu_info = "";
        kernel_info = "";

        disk_total = 0;
        disk_used = 0;

        ram_total = 0;
        ram_used = 0;
    }
};

class device_process final
{
public:
    /// \brief constructor
    device_process() = default;
    device_process(const device_process&)=delete;

    /// - destructor, delete not allow for singleton pattern.
    virtual ~device_process() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static device_process* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_buffer
    /// - This method is used to sendbuffer to local device
    /// \param ptr - point to send buffer
    /// \param size - size of the send buffer
    /// \return buffer send size.
    int send_buffer(const char* ptr, int size);

    /// \brief system_info_init
    /// - This method is used to do system information init
    /// \return buffer send size.
    bool system_info_init(void);

    /// \brief update_device_string
    /// - update device string info.
    void update_info_string();

     /// \brief set_device
    /// - set device command.   
    int set_device(int dev, char *buf, int size);

    /// \brief sync_info
    /// - sync info command. 
    int sync_info(char *buf, int size);
    
private:
    /// \brief run
    /// - This method is used for thread run the device management.
    void run();

    /// \brief get_cpu_info
    /// - This method is used for get cpu info.
    /// \return true - success, false - failed
    bool get_cpu_info();

    /// \brief get_kernel_info
    /// - This method is used for get kernel info.
    /// \return true - success, false - failed
    bool get_kernel_info();

    /// \brief get_disk_info
    /// - This method is used for get kernel info.
    /// \return true - success, false - failed
    bool get_disk_info();

    /// \brief get_ram_info
    /// - This method is used for get kernel info.
    /// \return true - success, false - failed
    bool get_ram_info();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static device_process* instance_pointer_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_cmd_fifo_point_;

    /// \brief device_fifo_point_
    /// - fifo point used for the device management.
    std::unique_ptr<fifo_manage> device_info_fifo_point_;

    /// \brief rx_buffer_
    /// - buffer used to store rx command.
    char rx_buffer_[DEVICE_RX_BUFFER_SIZE];

    /// \brief tx_buffer_
    /// - buffer used to store tx info.
    char tx_buffer_[DEVICE_TX_BUFFER_SIZE];

    /// \brief info_
    /// - buffer used to store device read info.
    device_read_info info_;

    /// \brief sysinfo_
    /// - buffer used to store local sys information.
    local_sysinfo sysinfo_;

    /// \brief info_str_
    /// - the info string buffer
    std::string info_str_;
};