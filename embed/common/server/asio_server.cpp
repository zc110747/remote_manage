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
#include "timer_manage.hpp"
#include "logger_manage.hpp"

///////////////////////////// session group ////////////////////////
void session_group::init(std::function<void(char* ptr, int size)> handler)
{
    set_.clear();
    handler_ = handler;
}

void session_group::join(share_session_pointer cur_session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    set_.insert(cur_session);
}

void session_group::leave(share_session_pointer cur_session)
{
    std::lock_guard<std::mutex> lock(mutex_);
    set_.erase(cur_session);
}

share_session_pointer session_group::get_session()
{
    share_session_pointer current_Session;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (set_.size() == 0)
            current_Session = nullptr;
        else
            current_Session = *set_.begin(); //only send message to first session
    }
    return current_Session;
}

const std::set<share_session_pointer>& session_group::get_session_list() 
{
    return set_;
}

void session_group::run(char *pbuf, int size)
{
    handler_(pbuf, size);
}

bool session_group::is_valid()
{
    if (set_.size() != 0) 
    {
        return true;
    }

    return false;
}

void session_group::do_write(const char *buffer, int size)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (set_.size() != 0)
    {
        share_session_pointer session_ptr = *set_.begin();  
        session_ptr->do_write(buffer, size);
    }
}

///////////////////////////// session ////////////////////////
void session::start()
{
    group_.join(shared_from_this());
    do_read();
}

void session::do_read()
{
    auto self(shared_from_this());
    socket_.async_read_some(asio::buffer(data_, MAX_LENGTH),
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

void session::do_write(const char *pdate, std::size_t length)
{
    auto self(shared_from_this());

    asio::async_write(socket_, asio::buffer(pdate, length),
        [this, self](std::error_code ec, std::size_t /*length*/)
        {
        if (!ec)
        {
            //do write callback
        }
        else
        {
            group_.leave(shared_from_this());
        }
        });
}

void session::do_close()
{
    socket_.close();
}

///////////////////////////// asio server ////////////////////////
void asio_server::run()
{
    io_context_.run();
}

const std::set<share_session_pointer>& asio_server::get_session_list()
{
    return group_.get_session_list();
}

share_session_pointer asio_server::get_valid_session()
{
    return group_.get_session();
}

void asio_server::close_all_session()
{
    auto session = group_.get_session();
    while (session != nullptr)
    {
        session->do_close();
        group_.leave(session);
        session = group_.get_session();
    }
}

bool asio_server::is_valid()
{
    return group_.is_valid();
}

void asio_server::do_write(const char *buffer, int size)
{
    group_.do_write(buffer, size);
}

void asio_server::init(const std::string& address, const std::string& port, std::function<void(char* ptr, int size)> handler)
{
    //update for rx handler
    group_.init(handler);

    PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "asio_server start, bind:%s:%s!", address.c_str(), port.c_str());
    
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
                close_all_session();
                std::make_shared<session>(std::move(socket), group_)->start();
                PRINT_LOG(LOG_FATAL, xGetCurrentTimes(), "Connect from client!");
            }

            do_accept();
        });
}
