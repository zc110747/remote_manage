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

using asio::ip::tcp;

class session
  : public std::enable_shared_from_this<session>
{
public:
  session(tcp::socket socket)
    : socket_(std::move(socket))
  {
  }

  void start()
  {
    do_read();
  }

private:
  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
          if (!ec)
          {
            do_write(length);
          }
        });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data_, length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            do_read();
          }
        });
  }

  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
};

asio_server::asio_server(const std::string& address, const std::string& port)
:io_context_(1), 
 signals_(io_context_),
 acceptor_(io_context_)
{
    //do_await_stop();

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    asio::ip::tcp::resolver resolver(io_context_);
    asio::ip::tcp::endpoint endpoint =
        *resolver.resolve(address, port).begin();
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    
    PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "axio_server start!");
    do_accept();
} 

void asio_server::run()
{
  // The io_context::run() call will block until all asynchronous operations
  // have finished. While the server is running, there is always at least one
  // asynchronous operation outstanding: the asynchronous accept call waiting
  // for new incoming connections.
  io_context_.run();
}

void asio_server::do_accept()
{
  acceptor_.async_accept(
      [this](std::error_code ec, asio::ip::tcp::socket socket)
      {
        // Check whether the server was stopped by a signal before this
        // completion handler had a chance to run.
        if (!acceptor_.is_open())
        {
          return;
        }

        if (!ec)
        {
            std::make_shared<session>(std::move(socket))->start();
        }

        do_accept();
      });
}

void asio_server::do_await_stop()
{
  signals_.async_wait(
      [this](std::error_code /*ec*/, int /*signo*/)
      {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_context::run()
        // call will exit.
        acceptor_.close();
      });
}