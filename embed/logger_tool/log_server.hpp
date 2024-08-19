//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      log_server.hpp
//
//  Purpose:
//      基于asio_server实现的tcp服务器, 提供命令的接收处理以及异步的logger打印接口
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

#define LOGGER_MESSAGE_BUFFER_SIZE  16384

class log_server final
{
public:
    /// \brief constructor
    log_server() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~log_server() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static log_server *get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief send_buffer
    /// - This method is send buffer throught fifo.
    /// \param pbuffer - point to start of the send buffer.
    /// \param time - size of the send buffer.
    int send_buffer(char *pbuffer, uint32_t size);
    
private:
    /// \brief logger_rx_server_run
    /// - rx thread to process logger interface received.
    void logger_rx_server_run();

    /// \brief logger_file_process
    /// - used to logger file saved.
    /// \param pbuffer - point to start of the save buffer.
    /// \param time - size of the save buffer.
    /// \return wheater save is ok or not
    int logger_file_process(char *buffer, uint32_t size);

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static log_server *instance_pointer_;

    /// \brief thread_work_
    /// - wheather thread is work.
    std::atomic<bool> thread_work_{false};

    /// \brief logger_rx_thread_
    /// - logger rx thread object.
    std::thread logger_rx_thread_;

    /// \brief outfile
    /// - file to read.
    std::ofstream outfile;
};


