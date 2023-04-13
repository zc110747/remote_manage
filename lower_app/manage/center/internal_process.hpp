//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      internal_process.hpp
//
//  Purpose:
//      用于内部通讯的交互，负责和node服务器，GUI显示等外部进程的交互
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/20/2022   Create New Version	
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "includes.hpp"
#include "logger.hpp"
#include "modules.hpp"

class internal_process final
{
public:
	/// \brief constructor
    internal_process() = default;

    /// - destructor, delete not allow for singleton pattern.
    ~internal_process() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static internal_process* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief update_device_status
    /// - This method is used to update the device info to other interface.
    /// convert to string "!status led=ON;BEEP=OFF;IR=1;ALS=1;PS=1;gypox=0;gypoz=0;gypoz=0".
    /// \param info - object of the device info.
    void update_device_status(const NAMESPACE_DEVICE::device_read_info &info);
    
private:
    /// \brief run
    /// - This method is used for thread run the internal process.
    void run();

    /// \brief send
    /// - This method is used to send data to all connected interface.
    /// \param pbuffer - buffer point to the start of the send buffer.
    /// \param size - size need to send.
    /// \return wheather already send the buffer success.
    bool send(char *pbuffer, int size);

    /// \brief process_info_callback
    /// - This method is used to process the extend action 
    /// if received event.
    void process_info_callback();

private:
    /// \brief pInstance
    /// - object used to implement the singleton pattern.
    static internal_process*  pInstance;

    /// \brief node_thread_
    /// - object used to saved thread information used to process.
    /// node communication.
    std::thread node_thread_;

    /// \brief cmd_process_
    /// - object used to parse command with structure "![do] [data]".
    cmd_process cmd_process_;
};