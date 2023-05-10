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

class session;
using share_session_pointer = std::shared_ptr<session>;
class session_group
{
public:
    void init(std::function<void(char* ptr, int size)>);
    void do_write(char *buffer, int size);
    void join(share_session_pointer Session_);
    void leave(share_session_pointer Session_);
    share_session_pointer get_session();
    const std::set<share_session_pointer>& get_session_list();
    void run(char *pbuf, int size);
    bool is_valid();
    
private:
    std::mutex mutex_;
    std::set<share_session_pointer> set_;
    std::function<void(char* ptr, int size)> handler_;
};

class session : public std::enable_shared_from_this<session>
{
public:
  session(asio::ip::tcp::socket socket, session_group& group)
  :socket_(std::move(socket)), group_(group)
  {}

  void start();

  void do_read();

  void do_write(std::size_t length);
  
  void do_write(char *pdate, std::size_t length);

  void do_close();

private:
  asio::ip::tcp::socket socket_;

  enum { max_length = 1024 };
  char data_[max_length];

  session_group& group_;
};

class asio_server
{
public:
    asio_server(const asio_server&) = delete;
    asio_server& operator=(const asio_server&) = delete;

    explicit asio_server()
    :io_context_(2), acceptor_(io_context_)
    {}

    void run();

    const std::set<share_session_pointer>& get_session_list();

    share_session_pointer get_valid_session();

    void clearSocket();

    bool is_valid();

    void do_write(char *buffer, int size);

    void init(const std::string& address, const std::string& port, std::function<void(char* ptr, int size)> handler);
private:
    void do_accept();
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    session_group group;
};