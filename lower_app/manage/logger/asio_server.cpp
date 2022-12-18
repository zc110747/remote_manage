//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      asio_server.cpp
//
//  Purpose:
//      support for tcp server by asio
//
// Author:
//      Alva Zhange
//
//  Assumptions:
//
//  Revision History:
//      8/8/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "asio_server.hpp"
#include "logger.hpp"

void asio_server::init(const std::string& address, const std::string& port, std::function<void(char* ptr, int size)> handler)
{
    //update for rx handler
    group.init(handler);

    PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "asio_server start, bind:%s:%s!", address.c_str(), port.c_str());
    
    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::endpoint endpoint =
        *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    
    do_accept();
} 

void asio_server::do_accept()
{
  acceptor_.async_accept(
      [this](std::error_code ec, asio::ip::tcp::socket socket)
      {
        if (!acceptor_.is_open())
        {
          return;
        }

        if (!ec)
        {
            std::make_shared<session>(std::move(socket), group)->start();
            PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Connect from client!");
        }

        do_accept();
      });
}
