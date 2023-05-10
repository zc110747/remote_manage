//////////////////////////////////////////////////////////////////////////////
//  (c) copyright 2022-by Persional Inc.  
//  All Rights Reserved
//
//  Name:
//      SocketTcpThread.cpp
//
//  Purpose:
//      TCP协议执行模块，目前设计为同步模式，后续会修改
//
// Author:
//     	@听心跳的声音
//
//  Assumptions:
//
//  Revision History:
//      12/19/2022   Create New Version
/////////////////////////////////////////////////////////////////////////////
#include "tcp_thread.hpp"
#include "asio_server.hpp"

static asio_server socket_tcp_server;

void tcp_thread_manage::tcp_server_run()
{
    const auto& socket_config = system_config::get_instance()->get_tcp_config();
    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "tcp info:%s:%d", socket_config.ipaddr.c_str(), socket_config.port);
    try
    {
        socket_tcp_server.init(socket_config.ipaddr, std::to_string(socket_config.port), [this](char* ptr, int length){
            tcp_protocol_pointer_->write_rx_fifo(ptr, length);
        });
        socket_tcp_server.run();
    }
    catch (std::exception& e)
    {
        PRINT_LOG(LOG_DEBUG, xGetCurrentTicks(), "tcp_thread_manage, Exception:%s", e.what());
    }
}

void tcp_thread_manage::tcp_rx_run()
{
    char data;
    ENUM_PROTOCOL_STATUS status;

    while(1)
    {
        if(tcp_protocol_pointer_->read_rx_fifo(&data, 1) > 0)
        {
            status = tcp_protocol_pointer_->check_rx_frame(data);
            if(status == ROTOCOL_FRAME_FINISHED)
            {
                //if process, clear data received.
                tcp_protocol_pointer_->process_rx_frame();
                tcp_protocol_pointer_->clear_rx_info();
            }
            else if(status == PROTOCOL_FRAME_EMPTY)
            {
                tcp_protocol_pointer_->clear_rx_info();
            }
            else
            {
                //in receive, do nothing
            }
        }
    }
}

int tcp_thread_manage::send_msg(char *buffer, uint16_t size)
{
    if(socket_tcp_server.is_valid())
    {
        return tcp_protocol_pointer_->send_data(buffer, size);
    }
    return -1;
}

void tcp_thread_manage::tcp_tx_run()
{
    ENUM_PROTOCOL_STATUS status;
    int size;
    char buffer[TX_BUFFER_SIZE];

    while(1)
    {
        size = tcp_protocol_pointer_->read_tx_fifo(buffer, TX_BUFFER_SIZE);
        if(size > 0)
        {
            if(socket_tcp_server.is_valid())
            {
                socket_tcp_server.do_write(buffer, size);
            }
        }
        else
        {
            //do nothing
        }
    }
}

bool tcp_thread_manage::init()
{
    //must creat fifo before thread start
    tcp_protocol_pointer_ = std::make_unique<protocol_info>();
    if(tcp_protocol_pointer_ == nullptr)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "tcp_protocol_pointer_ create failed!");
        return false;
    }

    auto ret = tcp_protocol_pointer_->init(SOCKET_TCP_RX_FIFO, SOCKET_TCP_TX_FIFO,  [](char *ptr, int size){
        socket_tcp_server.do_write(ptr, size);
    });
    if(!ret)
    {
        PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "tcp_protocol_pointer_ init failed!");
        return false;
    }

    tcp_server_thread_ = std::thread(std::bind(&tcp_thread_manage::tcp_server_run, this));
    tcp_server_thread_.detach();
    tcp_rx_thread_ = std::thread(std::bind(&tcp_thread_manage::tcp_rx_run, this));
    tcp_rx_thread_.detach();
    tcp_tx_thread_ = std::thread(std::bind(&tcp_thread_manage::tcp_tx_run, this));
    tcp_tx_thread_.detach();

    PRINT_LOG(LOG_INFO, xGetCurrentTicks(), "tcp thread init success!");
    return true;
}

tcp_thread_manage* tcp_thread_manage::instance_pointer_ = nullptr;
tcp_thread_manage* tcp_thread_manage::get_instance()
{
    if(instance_pointer_ == nullptr)
    {
        instance_pointer_ = new(std::nothrow) tcp_thread_manage;
        if(instance_pointer_ == nullptr)
        {
            PRINT_LOG(LOG_ERROR, xGetCurrentTicks(), "Tcp thread manage new failed!");
        }
    }
    return instance_pointer_;
}