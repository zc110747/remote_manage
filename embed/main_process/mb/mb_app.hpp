//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2023-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      mb_app.h
//
//  Purpose:
//      interface for process modbus protocol task.
//
//  Author:
//      @zc
//
//  Assumptions:
//	
//  Revision History:
//
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common.hpp"

class mb_app_manage
{
public:
    /// \brief constructor
    mb_app_manage() = default;

    /// \brief destructor, delete not allow for singleton pattern.
    ~mb_app_manage() = delete;

    /// \brief get_instance
    /// - This method is used to get the pattern of the class.
    /// \return the singleton pattern point of the object.
    static mb_app_manage* get_instance();

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init();

private:
    /// \brief mb_workflow_run
    /// - server thread to process tcp received, tranform to rx fifo.
    void mb_workflow_run();

private:
    /// \brief instance_pointer_
    /// - object used to implement the singleton pattern.
    static mb_app_manage* instance_pointer_;

    /// \brief tcp_server_thread_
    /// - tcp server thread object.
    std::thread mb_workflow_thread_;
};