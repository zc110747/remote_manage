//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      logger_process.hpp
//
//  Purpose:
//      logger fifo manage.
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

class log_process final
{
public:
    /// \brief constructor
    log_process() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~log_process() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static log_process* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

    /// \brief release
    /// - This method is used to release the object.
    /// \return Wheather release is success or failed.
    void release();

private:
    /// \brief logger_rx_run
    /// - rx thread to process logger fifo rx.
    void logger_rx_run();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static log_process *instance_pointer_;

    /// \brief logger_rx_thread_
    /// - logger rx thread object. 
    std::thread logger_rx_thread_;

    /// \brief logger_internal_tx_fifo_
    /// - fifo used for logger manage.
    std::unique_ptr<fifo_manage> logger_rx_fifo_{nullptr};
};
