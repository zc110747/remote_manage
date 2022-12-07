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
#include <memory>
#include <iostream>
#include <set>

class session;
using share_session_ptr = std::shared_ptr<session>;
class session_manage
{
public:
    void init(std::function<void(char* ptr, int size)> handler)
    {
        set_.clear();
        handler_ = handler;
    }

    void join(share_session_ptr session_)
    {
        std::lock_guard<std::mutex> lock(mut_);
        set_.insert(session_);
    }

    void leave(share_session_ptr session_)
    {
        std::lock_guard<std::mutex> lock(mut_);
        set_.erase(session_);
    }

    share_session_ptr get_session()
    {
        share_session_ptr current_session;

        {
            std::lock_guard<std::mutex> lock(mut_);
            if(set_.size() == 0)
                current_session = nullptr;
            else
                current_session = *set_.begin();
        }
        return current_session;
    }

    void run(char *pbuf, int size)
    {
      handler_(pbuf, size);
    }

private:
    std::mutex mut_;
    std::set<share_session_ptr> set_;
    std::function<void(char* ptr, int size)> handler_;
};

class session : public std::enable_shared_from_this<session>
{
public:
  session(asio::ip::tcp::socket socket, session_manage& group)
    : socket_(std::move(socket)),
      group_(group)
  {
  }

  void start()
  {
    group_.join(shared_from_this());
    do_read();
  }

  void do_read()
  {
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, max_length),
        [this, self](std::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                group_.run(data_, length);
                do_read();
            }
            else
            {
                group_.leave(shared_from_this());
                std::cout<<ec<<"\n";
            }
        });
  }

  void do_write(char *pdate, std::size_t length)
  {
    auto self(shared_from_this());

    asio::async_write(socket_, asio::buffer(pdate, length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if(!ec)
          {
              //do write callback
          }
          else
          {
            group_.leave(shared_from_this());
          }
        });
  }

  void do_write(std::size_t length)
  {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(data_, length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
          if(!ec)
          {
              //do write callback
          }
          else
          {
            group_.leave(shared_from_this());
          }
        });
  }

private:
  asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  session_manage& group_;
};


class asio_server
{
public:
    asio_server(const asio_server&) = delete;
    asio_server& operator=(const asio_server&) = delete;

    explicit asio_server():io_context_(2), acceptor_(io_context_)
    {
    }

    void run(){
        io_context_.run();
    }

    share_session_ptr get_valid_session()
    {
        return group.get_session();
    }

    void init(const std::string& address, const std::string& port, std::function<void(char* ptr, int size)> handler);
private:
    void do_accept();
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    session_manage group;
};