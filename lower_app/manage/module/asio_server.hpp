//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      asio_server.hpp
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
_Pragma("once")
#if !__has_include("asio.hpp")
#error "asio library not exist, need release in lib directory"
#endif

#include "asio.hpp"
#include <set>
#include <memory>
#include <iostream>
#include <mutex>

class Session;
using ShareSessionPointer = std::shared_ptr<Session>;
class SessionMessage
{
public:
    void init(std::function<void(char* ptr, int size)> handler)
    {
        set_.clear();
        handler_ = handler;
    }

    void join(ShareSessionPointer Session_)
    {
        std::lock_guard<std::mutex> lock(mut_);
        set_.insert(Session_);
    }

    void leave(ShareSessionPointer Session_)
    {
        std::lock_guard<std::mutex> lock(mut_);
        set_.erase(Session_);
    }

    ShareSessionPointer get_session()
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

    const std::set<ShareSessionPointer>& get_session_list() {
      return set_;
    }

    void run(char *pbuf, int size)
    {
      handler_(pbuf, size);
    }

private:
    std::mutex mut_;
    std::set<ShareSessionPointer> set_;
    std::function<void(char* ptr, int size)> handler_;
};

class Session : public std::enable_shared_from_this<Session>
{
public:
  Session(asio::ip::tcp::socket socket, SessionMessage& group)
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
                data_[length] = 0;
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

  void do_close()
  {
    socket_.close();
  }
private:
  asio::ip::tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  SessionMessage& group_;
};


class AsioServer
{
public:
    AsioServer(const AsioServer&) = delete;
    AsioServer& operator=(const AsioServer&) = delete;

    explicit AsioServer():io_context_(2), acceptor_(io_context_)
    {
    }

    void run(){
        io_context_.run();
    }

    const std::set<ShareSessionPointer>& get_session_list()
    {
        return group.get_session_list();
    }

    ShareSessionPointer get_valid_session()
    {
        return group.get_session();
    }

    void clearSocket()
    {
      auto session = group.get_session();
      while(session != nullptr)
      {
          session->do_close();
          group.leave(session);
          session = group.get_session();
      }
    }

    void init(const std::string& address, const std::string& port, std::function<void(char* ptr, int size)> handler);
private:
    void do_accept();
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    SessionMessage group;
};