//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      asio_server.hpp
//
//  Purpose:
//      support for tcp server by asio.
//      this copy from the example by asio application.
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      8/8/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////

#include "asio.hpp"

class asio_server
{
public:
    asio_server(const asio_server&) = delete;
    asio_server& operator=(const asio_server&) = delete;

    /// Construct the server to listen on the specified TCP address and port, and
    /// serve up files from the given directory.
    explicit asio_server(const std::string& address, const std::string& port);

    /// Run the server's io_context loop.
    void run();

private:
    /// Perform an asynchronous accept operation.
    void do_accept();

    /// Wait for a request to stop the server.
    void do_await_stop();

    /// The io_context used to perform asynchronous operations.
    asio::io_context io_context_;

    /// The signal_set is used to register for process termination notifications.
    asio::signal_set signals_;

    /// Acceptor used to listen for incoming connections.
    asio::ip::tcp::acceptor acceptor_;
};