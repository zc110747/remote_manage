//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.
//  All Rights Reserved
//
//  Name:
//      can_socket.hpp
//
//  Purpose:
//      CAN通讯管理模块，应用处理
//
// Author:
//      @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
_Pragma("once")

#include "common_unit.hpp"

class can_socket final
{
public:
    /// \brief can_socket
    can_socket()  {}

    /// \brief can_socket
    can_socket(const std::string &interface, int local_id = 0)
    : interface_(interface), local_id_(local_id)
    {}

    /// \brief init
    /// - This method is used to init the object.
    /// \return Wheather initialization is success or failed.
    bool init(void);

    /// \brief release
    /// - This method is used to release the object.
    void release(void) {
        if (socket_fd_ == -1) {
            close(socket_fd_);
            socket_fd_ = -1;
        }
    }
    
    /// \brief set_interface
    /// - This method is used to set the interface name.
    /// \param interface - the interface name.
    void set_interface(const std::string &interface) {
        interface_ = interface;
    }

    /// \brief is_socket_init
    /// - This method is used to check if socket is init.
    /// \return Wheather socket is init or not.
    bool is_socket_init(void) {
        return (socket_fd_ != -1);
    }

    /// \brief destructor
    ~can_socket() {}

private:
    /// \brief run
    /// - This method is used to run can socket process.
    void run(void);

    /// \brief write_buffer
    /// - This method is used to write buffer to can socket.
    /// \param buffer - the buffer to write.
    /// \param size - the size of buffer.
    /// \return Wheather write is success or failed.
    bool write_buffer(uint8_t *buffer, uint32_t size);

private:
    /// \brief socket_fd_
    /// - this member is used to store the socket fd.
    int socket_fd_{-1};

    /// \brief interface_
    /// - this member is used to store the interface name.
    std::string interface_{""};

    /// \brief local_id_
    /// - this member is used to store the local id.
    int local_id_{0};
};
