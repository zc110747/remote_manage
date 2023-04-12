//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      asio_server.cpp
//
//  Purpose:
//      基于asio实现的tcp服务器, 单连接，有新连接则断开旧连接
//
// Author:
//     @听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version	
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
            //if accept, close other socket
            clearSocket();
            std::make_shared<Session>(std::move(socket), group)->start();
            PRINT_LOG(LOG_FATAL, xGetCurrentTicks(), "Connect from client!");
        }

        do_accept();
      });
}


//session_group
void session_group::init(std::function<void(char* ptr, int size)> handler)
{
  set_.clear();
  handler_ = handler;
}

void session_group::join(ShareSessionPointer Session_)
{
  std::lock_guard<std::mutex> lock(mut_);
  set_.insert(Session_);
}

void session_group::leave(ShareSessionPointer Session_)
{
  std::lock_guard<std::mutex> lock(mut_);
  set_.erase(Session_);
}

ShareSessionPointer session_group::get_session()
{
    ShareSessionPointer current_Session;

    {
        std::lock_guard<std::mutex> lock(mut_);
        if(set_.size() == 0)
            current_Session = nullptr;
        else
            current_Session = *set_.begin(); //only send message to first Session
    }
    return current_Session;
}

const std::set<ShareSessionPointer>& session_group::get_session_list() 
{
  return set_;
}

void session_group::run(char *pbuf, int size)
{
  handler_(pbuf, size);
}

bool session_group::is_valid()
{
  if(set_.size() != 0)
    return true;
  return false;
}

void session_group::do_write(char *buffer, int size)
{
  std::lock_guard<std::mutex> lock(mut_);
  if(set_.size() != 0)
  {
    ShareSessionPointer session_ptr = *set_.begin();  
    session_ptr->do_write(buffer, size);
  }
}

